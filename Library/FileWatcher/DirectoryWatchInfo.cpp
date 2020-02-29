#include "StdAfx.h"
#include "DirectoryWatchInfo.h"

CDirWatchInfo::CDirWatchInfo(HANDLE hDiretory, LPCTSTR lpszDirectoryName,  CDirectoryChangeHandler * pChangeHandler,
												DWORD dwChangeFilter,  BOOL bWatchSubDir, BOOL bAppHasGUI,
												LPCTSTR szIncludeFilter, LPCTSTR szExcludeFilter, DWORD dwFilterFlags)
		 : m_pChangeHandler(NULL)
		 , m_hDirectory(hDiretory)
		 , m_dwChangeFilter(dwChangeFilter)
		 , m_bWatchSubDir(bWatchSubDir)
		 , m_strDirName(lpszDirectoryName)
		 , m_dwBufLength(0)
		 , m_dwReadDirError(ERROR_SUCCESS)
		 , m_StartStopEvent(FALSE, TRUE) //NOT SIGNALLED, MANUAL RESET
		 , m_RunningState(RUNNING_STATE_NOT_SET)
{ 
	ASSERT(m_hDirectory != INVALID_HANDLE_VALUE && _tcslen(lpszDirectoryName) != 0);
	
	m_pChangeHandler = new CDelayedDirectoryChangeHandler(pChangeHandler, bAppHasGUI, szIncludeFilter, szExcludeFilter, dwFilterFlags);
	if(m_pChangeHandler) 
		m_pChangeHandler->SetPartialPathOffset(m_strDirName);//to support FILTERS_CHECK_PARTIAL_PATH..this won't change for the duration of the watch, so set it once... HERE!
	ASSERT(m_pChangeHandler);

	ASSERT(GetChangeHandler());
	ASSERT(GetRealChangeHandler());
	if(GetRealChangeHandler())
		GetRealChangeHandler()->AddRef();
	
	memset(&m_Overlapped, 0, sizeof(m_Overlapped));
	//memset(m_Buffer, 0, sizeof(m_Buffer));
}

CDirWatchInfo::~CDirWatchInfo()
{
	//If this call to CDirectoryChangeHandler::Release() causes m_pChangeHandler to delete itself,
	//the dtor for CDirectoryChangeHandler will call CDirectoryChangeWatcher::UnwatchDirectory( CDirectoryChangeHandler * ),
	//which will make try to delete this object again.
	//if m_pChangeHandler is NULL, it won't try to delete this object again...
	if (GetChangeHandler())
	{
		CDirectoryChangeHandler * pHandler = SetRealDirectoryChangeHandler(NULL);
		if(pHandler)
			pHandler->Release();
		else{
			ASSERT( FALSE );
		}
	}
	
	CloseDirectoryHandle();

	delete m_pChangeHandler;
	m_pChangeHandler = NULL;	
}

//
//	There's a reason for this function!
//
//	the dtor is private to enforce that it is used.
//
//
//	pWatcher can be NULL only if CDirecotryChangeHandler::ReferencesWatcher() has NOT been called.
//	ie: in certain sections of WatchDirectory() it's ok to pass this w/ NULL, but no where else.
//
void CDirWatchInfo::DeleteSelf(CDirectoryChangeWatcher * pWatcher)
{
	//ASSERT( pWatcher != NULL );
	ASSERT(GetRealChangeHandler());

	//
	//
	//	Before this object is deleted, the CDirectoryChangeHandler object
	//	needs to release it's reference count to the CDirectoryChangeWatcher object.
	//	I might forget to do this since I getting rid of CDirWatchInfo objects
	//	in more than one place...hence the reason for this function.
	//
	if (pWatcher)
		pWatcher->ReleaseReferenceToWatcher(GetRealChangeHandler());
	
	delete this;
}

CDelayedDirectoryChangeHandler* CDirWatchInfo::GetChangeHandler() const 
{ 
	return m_pChangeHandler; 
}

//
//	The 'real' change handler is the CDirectoryChangeHandler object 
//	passed to CDirectoryChangeWatcher::WatchDirectory() -- this is the object
//	that really handles the changes.
//
CDirectoryChangeHandler * CDirWatchInfo::GetRealChangeHandler() const
{	
	ASSERT( m_pChangeHandler ); 
	return m_pChangeHandler->GetRealChangeHandler(); 
}

//
//	Allows you to switch out, at run time, which object really handles the change notifications.
//
CDirectoryChangeHandler * CDirWatchInfo::SetRealDirectoryChangeHandler(CDirectoryChangeHandler * pChangeHandler)
{
	CDirectoryChangeHandler * pOld = GetRealChangeHandler();
	m_pChangeHandler->GetRealChangeHandler() = pChangeHandler; 
	return pOld;
}

//
//	Closes the directory handle that was opened in CDirectoryChangeWatcher::WatchDirecotry()
//
//
BOOL CDirWatchInfo::CloseDirectoryHandle()
{
	BOOL bRet = TRUE;
	if( m_hDirectory != INVALID_HANDLE_VALUE )
	{
		bRet = CloseHandle(m_hDirectory);
		m_hDirectory = INVALID_HANDLE_VALUE;
	}
	return bRet;
}

CString& CDirWatchInfo::GetDirectoryName()
{
	return m_strDirName;
}

/*********************************************
  Sets the running state of the object to perform the initial call to ReadDirectoryChangesW()
  , wakes up the thread waiting on GetQueuedCompletionStatus()
  and waits for an event to be set before returning....

  The return value is either ERROR_SUCCESS if ReadDirectoryChangesW is successfull,
  or is the value of GetLastError() for when ReadDirectoryChangesW() failed.
**********************************************/
DWORD CDirWatchInfo::StartMonitor(HANDLE hCompPort)
{
	ASSERT(hCompPort);

	//Guard the properties of this object 
	VERIFY(LockProperties());
	
	m_RunningState = RUNNING_STATE_START_MONITORING;//set the state member to indicate that the object is to START monitoring the specified directory
	PostQueuedCompletionStatus(hCompPort, sizeof(this), (ULONG_PTR)this, &m_Overlapped);//make the thread waiting on GetQueuedCompletionStatus() wake up

	VERIFY(UnlockProperties());//unlock this object so that the thread can get at them...

	//wait for signal that the initial call 
	//to ReadDirectoryChanges has been made
	DWORD dwWait = 0;
	do{
		dwWait = WaitForSingleObject(m_StartStopEvent, 10 * 1000);
		if( dwWait != WAIT_OBJECT_0 )
		{
			TRACE(_T("WARNING! Possible lockup detected. FILE: %s Line: %d\n"), _T( __FILE__ ), __LINE__);
		}
	} while( dwWait != WAIT_OBJECT_0 );

	ASSERT( dwWait == WAIT_OBJECT_0 );
	m_StartStopEvent.ResetEvent();
	
	return m_dwReadDirError;//This value is set in the worker thread when it first calls ReadDirectoryChangesW().
}

/*******************************************

    Sets the running state of the object to stop monitoring a directory,
	Causes the worker thread to wake up and to stop monitoring the dierctory
	
********************************************/
BOOL CDirWatchInfo::UnwatchDirectory(HANDLE hCompPort)
{
	ASSERT(hCompPort);

			//and wait for the thread to signal that it has shutdown
	if(SignalShutdown(hCompPort))
	{
		return WaitForShutdown();
	}
	return FALSE;
}

void CDirWatchInfo::SetReadDirectoryStatus(DWORD dwReadDirectoryStatus /*= 0*/)
{
	m_dwReadDirError = dwReadDirectoryStatus;
}

DWORD CDirWatchInfo::GetReadDirectoryStatus() const
{
	return m_dwReadDirError;
}

RUNNING_STATE CDirWatchInfo::GetRunState() const
{
	return m_RunningState;
}

HANDLE CDirWatchInfo::GetDirectoryHANDLE() const
{
	return m_hDirectory;
}

//added to fix a bug -- this will be called normally by UnwatchDirectory(HANDLE)
//						and abnormally by the worker thread in the case that ReadDirectoryChangesW fails -- see code.
//
//	Signals the worker thread(via the I/O completion port) that it is to stop watching the 
//	directory for this object, and then returns.
//
BOOL CDirWatchInfo::SignalShutdown( HANDLE hCompPort )
{
	BOOL bRetVal = FALSE;
	ASSERT(hCompPort);
	ASSERT(m_hDirectory != INVALID_HANDLE_VALUE);
	//Lock the properties so that they aren't modified by another thread
	VERIFY( LockProperties() ); //unlikey to fail...
		
	//set the state member to indicate that the object is to stop monitoring the 
	//directory that this CDirWatchInfo is responsible for...
	m_RunningState = RUNNING_STATE_STOP;
	//put this object in the I/O completion port... GetQueuedCompletionStatus() will return it inside the worker thread.
	bRetVal = PostQueuedCompletionStatus(hCompPort, sizeof(CDirWatchInfo*), (ULONG_PTR)this, &m_Overlapped);

	if( !bRetVal )
	{
		TRACE(_T("PostQueuedCompletionStatus() failed! GetLastError(): %d\n"), GetLastError());
	}
	VERIFY( UnlockProperties() );
	
	return bRetVal;
}

//
//	This is to be called some time after SignalShutdown().
//	
//
BOOL CDirWatchInfo::WaitForShutdown()
{
	ASSERT(&m_StartStopEvent);
	
	//Wait for the Worker thread to indicate that the watch has been stopped
	DWORD dwWait;
	bool bWMQuitReceived = false;
	do{
		HANDLE hObject = m_StartStopEvent.GetSafeHandle();
		dwWait	= MsgWaitForMultipleObjects(1, &hObject, FALSE, 5000, QS_ALLINPUT);//wait five seconds
		switch( dwWait )
		{
		case WAIT_OBJECT_0:
			//handle became signalled!
			break;
		case WAIT_OBJECT_0 + 1:
			{
				//This thread awoke due to sent/posted message
				//process the message Q
				//
				//	There is a message in this thread's queue, so 
				//	MsgWaitForMultipleObjects returned.
				//	Process those messages, and wait again.

				MSG msg;
				while( PeekMessage(&msg, NULL, 0,0, PM_REMOVE ) ) 
				{
					if( msg.message != WM_QUIT)
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					else
					{
						/****
						This appears to be causing quite a lot of pain, to quote Mustafa.

						//it's the WM_QUIT message, put it back in the Q and
						// exit this function
						PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam );
						bWMQuitReceived = true;

						****/
						break;
					}
				}
			}break;
		case WAIT_TIMEOUT:
			{
				TRACE(_T("WARNING: Possible Deadlock detected! ThreadID: %d File: %s Line: %d\n"), GetCurrentThreadId(), _T(__FILE__), __LINE__);
			}break;
		}//end switch(dwWait)
	}while( dwWait != WAIT_OBJECT_0 && !bWMQuitReceived );
		
	ASSERT( dwWait == WAIT_OBJECT_0 || bWMQuitReceived);

	m_StartStopEvent.ResetEvent();
	
	return (BOOL) (dwWait == WAIT_OBJECT_0 || bWMQuitReceived);
}