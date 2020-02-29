#include "StdAfx.h"
#include "DirectoryChangeWatcher.h"

//
//  Returns: bool
//		true if strPath is a path to a directory
//		false otherwise.
//
static BOOL IsDirectory(const CString & strPath)
{
	DWORD dwAttrib	= GetFileAttributes( strPath );
	return static_cast<BOOL>((dwAttrib != 0xffffffff && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)));
}

//
//	I think this code is from a Jeffrey Richter book...
//
//	Enables user priviledges to be set for this process.
//	
//	Process needs to have access to certain priviledges in order
//	to use the ReadDirectoryChangesW() API.  See documentation.
BOOL	EnablePrivilege(LPCTSTR pszPrivName, BOOL fEnable /*= TRUE*/) 
{    
	BOOL fOk = FALSE;    
	// Assume function fails    
	HANDLE hToken;    
	// Try to open this process's access token    
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) 	
	{        
		// privilege        
		TOKEN_PRIVILEGES tp = { 1 };        

		if( LookupPrivilegeValue(NULL, pszPrivName,  &tp.Privileges[0].Luid) )
		{
			tp.Privileges[0].Attributes = fEnable ?  SE_PRIVILEGE_ENABLED : 0;

			AdjustTokenPrivileges(hToken, FALSE, &tp, 			      
				sizeof(tp), NULL, NULL);

			fOk = (GetLastError() == ERROR_SUCCESS);		
		}
		CloseHandle(hToken);	
	}	
	return(fOk);
}

class CPrivilegeEnabler
{
private:
	CPrivilegeEnabler();//ctor
public:
	~CPrivilegeEnabler(){};

	static CPrivilegeEnabler & Instance();
	//friend static CPrivilegeEnabler & Instance();
};

CPrivilegeEnabler::CPrivilegeEnabler()
{
	LPCTSTR arPrivelegeNames[]	=	{
		SE_BACKUP_NAME, //	these two are required for the FILE_FLAG_BACKUP_SEMANTICS flag used in the call to 
		SE_RESTORE_NAME,//  CreateFile() to open the directory handle for ReadDirectoryChangesW

		SE_CHANGE_NOTIFY_NAME //just to make sure...it's on by default for all users.
		//<others here as needed>
	};
	for(int i = 0; i < sizeof(arPrivelegeNames) / sizeof(arPrivelegeNames[0]); ++i)
	{
		if(!EnablePrivilege(arPrivelegeNames[i], TRUE))
		{
			TRACE(_T("Unable to enable privilege: %s	--	GetLastError(): %d\n"), arPrivelegeNames[i], GetLastError());
			TRACE(_T("CDirectoryChangeWatcher notifications may not work as intended due to insufficient access rights/process privileges.\n"));
			TRACE(_T("File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		}
	}
}

CPrivilegeEnabler & CPrivilegeEnabler::Instance()
{
	static CPrivilegeEnabler theInstance;//constructs this first time it's called.
	return theInstance;
}

CDirectoryChangeWatcher::CDirectoryChangeWatcher(BOOL bAppHasGUI /*= TRUE*/, DWORD dwFilterFlags/*=FILTERS_CHECK_FILE_NAME_ONLY*/)
	: m_hCompPort(NULL) ,
	 m_hThread(NULL) ,
	 m_dwThreadID(0UL) ,
	 m_bAppHasGUI(bAppHasGUI) ,
	 m_dwFilterFlags(dwFilterFlags == 0? FILTERS_DEFAULT_BEHAVIOR : dwFilterFlags)
{

}

CDirectoryChangeWatcher::~CDirectoryChangeWatcher()
{
	UnwatchAllDirectories();

	if (m_hCompPort)
	{
		CloseHandle( m_hCompPort );
		m_hCompPort = NULL;
	}
}

DWORD CDirectoryChangeWatcher::SetFilterFlags(DWORD dwFilterFlags)
{
	DWORD dwFilter = m_dwFilterFlags;
	m_dwFilterFlags = dwFilterFlags;
	if (m_dwFilterFlags == 0)
		m_dwFilterFlags = FILTERS_DEFAULT_BEHAVIOR;//the default.
	return dwFilter;
}

/*********************************************
  Determines whether or not a directory is being watched

  be carefull that you have the same name path name, including the backslash
  as was used in the call to WatchDirectory().

	  ie:	
			"C:\\Temp"
		is different than
			"C:\\Temp\\"
**********************************************/
BOOL CDirectoryChangeWatcher::IsWatchingDirectory(LPCTSTR lpszDirToWatch) const
{
	CSingleLock lock( const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);
	ASSERT(lock.IsLocked());
	int n;
	if (GetDirWatchInfo(lpszDirToWatch, n))
		return TRUE;
	return FALSE;
}

int CDirectoryChangeWatcher::NumWatchedDirectories() const
{
	CSingleLock lock(const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);
	ASSERT( lock.IsLocked() );
	int nCnt(0), max = m_DirectoriesToWatch.GetSize();
	for(int n = 0; n < max; ++n)
	{
		if( m_DirectoriesToWatch[n] != NULL )//array may contain NULL elements.
			nCnt++;
	}

	return nCnt;
}

/*************************************************************
FUNCTION:	WatchDirectory(const CString & strDirToWatch,   --the name of the directory to watch
						   DWORD dwChangesToWatchFor, --the changes to watch for see dsk docs..for ReadDirectoryChangesW
						   CDirectoryChangeHandler * pChangeHandler -- handles changes in specified directory
						   BOOL bWatchSubDirs      --specified to watch sub directories of the directory that you want to watch
						   )

PARAMETERS:
		const CString & strDirToWatch -- specifies the path of the directory to watch.
		DWORD dwChangesToWatchFor	-- specifies flags to be passed to ReadDirectoryChangesW()
		CDirectoryChangeHandler *	-- ptr to an object which will handle notifications of file changes.
		BOOL bWatchSubDirs			-- specifies to watch subdirectories.
		LPCTSTR szIncludeFilter		-- A file pattern string for files that you wish to receive notifications
									   for. See Remarks.
		LPCTSTR szExcludeFilter		-- A file pattern string for files that you do not wish to receive notifications for. See Remarks

	Starts watching the specified directory(and optionally subdirectories) for the specified changes

	When specified changes take place the appropriate CDirectoryChangeHandler::On_Filexxx() function is called.

	dwChangesToWatchFor can be a combination of the following flags, and changes map out to the 
	following functions:
	FILE_NOTIFY_CHANGE_FILE_NAME    -- CDirectoryChangeHandler::On_FileAdded()
									   CDirectoryChangeHandler::On_FileNameChanged, 
									   CDirectoryChangeHandler::On_FileRemoved
	FILE_NOTIFY_CHANGE_DIR_NAME     -- CDirectoryChangeHandler::On_FileNameAdded(), 
									   CDirectoryChangeHandler::On_FileRemoved
	FILE_NOTIFY_CHANGE_ATTRIBUTES   -- CDirectoryChangeHandler::On_FileModified
	FILE_NOTIFY_CHANGE_SIZE         -- CDirectoryChangeHandler::On_FileModified
	FILE_NOTIFY_CHANGE_LAST_WRITE   -- CDirectoryChangeHandler::On_FileModified
	FILE_NOTIFY_CHANGE_LAST_ACCESS  -- CDirectoryChangeHandler::On_FileModified
	FILE_NOTIFY_CHANGE_CREATION     -- CDirectoryChangeHandler::On_FileModified
	FILE_NOTIFY_CHANGE_SECURITY     -- CDirectoryChangeHandler::On_FileModified?
	

	Returns ERROR_SUCCESS if the directory will be watched, 
	or a windows error code if the directory couldn't be watched.
	The error code will most likely be related to a call to CreateFile(), or 
	from the initial call to ReadDirectoryChangesW().  It's also possible to get an
	error code related to being unable to create an io completion port or being unable 
	to start the worker thread.

	This function will fail if the directory to be watched resides on a 
	computer that is not a Windows NT/2000/XP machine.


	You can only have one watch specified at a time for any particular directory.
	Calling this function with the same directory name will cause the directory to be 
	unwatched, and then watched again(w/ the new parameters that have been passed in).  

**************************************************************/
DWORD CDirectoryChangeWatcher::WatchDirectory(LPCTSTR lpszDirToWatch, DWORD dwChangesToWatchFor,  CDirectoryChangeHandler * pChangeHandler,
																				   BOOL bWatchSubDirs /*=FALSE*/, LPCTSTR szIncludeFilter /*=NULL*/, LPCTSTR szExcludeFilter /*=NULL*/)
{
	ASSERT( dwChangesToWatchFor != 0);

	if (lpszDirToWatch == _T('\0') ||  dwChangesToWatchFor == 0 ||  pChangeHandler == NULL)
	{
		TRACE(_T("ERROR: You've passed invalid parameters to CDirectoryChangeWatcher::WatchDirectory()\n"));
		::SetLastError(ERROR_INVALID_PARAMETER);
		return ERROR_INVALID_PARAMETER;
	}

	//double check that it's really a directory
	if (!IsDirectory(lpszDirToWatch))
	{
		TRACE(_T("ERROR: CDirectoryChangeWatcher::WatchDirectory() -- %s is not a directory!\n"), lpszDirToWatch);
		::SetLastError(ERROR_BAD_PATHNAME);
		return ERROR_BAD_PATHNAME;
	}

	//double check that this directory is not already being watched....
	//if it is, then unwatch it before restarting it...
	if (IsWatchingDirectory(lpszDirToWatch))
		VERIFY(UnwatchDirectory(lpszDirToWatch));
	//
	//
	//	Reference this singleton so that privileges for this process are enabled 
	//	so that it has required permissions to use the ReadDirectoryChangesW API, etc.
	//
	CPrivilegeEnabler::Instance();
	//
	//open the directory to watch....
	HANDLE hDirectory = CreateFile(lpszDirToWatch, 
															FILE_LIST_DIRECTORY, 
															FILE_SHARE_READ | FILE_SHARE_WRITE ,//| FILE_SHARE_DELETE, <-- removing FILE_SHARE_DELETE prevents the user or someone else from renaming or deleting the watched directory. This is a good thing to prevent.
															NULL,  OPEN_EXISTING,
															FILE_FLAG_BACKUP_SEMANTICS | //<- the required priviliges for this flag are: SE_BACKUP_NAME and SE_RESTORE_NAME.  CPrivilegeEnabler takes care of that.
															FILE_FLAG_OVERLAPPED, NULL);

	if (hDirectory == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		TRACE(_T("CDirectoryChangeWatcher::WatchDirectory() -- Couldn't open directory for monitoring. %d\n"), dwError);
		::SetLastError(dwError);//who knows if TRACE will cause GetLastError() to return success...probably won't, but set it manually just for fun.
		return dwError;
	}
	
	CDirWatchInfo * pDirInfo = new CDirWatchInfo(hDirectory, lpszDirToWatch, pChangeHandler, dwChangesToWatchFor, 
																	bWatchSubDirs, m_bAppHasGUI, szIncludeFilter, szExcludeFilter, m_dwFilterFlags);
	if( !pDirInfo )
	{
		TRACE(_T("WARNING: Couldn't allocate a new CDirWatchInfo() object --- File: %s Line: %d\n"), _T( __FILE__ ), __LINE__);
		CloseHandle( hDirectory );
		::SetLastError(ERROR_OUTOFMEMORY);
		return ERROR_OUTOFMEMORY;
	}
	
	//create a IO completion port/or associate this key with
	//the existing IO completion port
	m_hCompPort = CreateIoCompletionPort(hDirectory, 
																		m_hCompPort, //if m_hCompPort is NULL, hDir is associated with a NEW completion port,
																								 //if m_hCompPort is NON-NULL, hDir is associated with the existing completion port that the handle m_hCompPort references
																		(DWORD)pDirInfo, //the completion 'key'... this ptr is returned from GetQueuedCompletionStatus() when one of the events in the dwChangesToWatchFor filter takes place
																		0);
	if (m_hCompPort == NULL) {
		TRACE(_T("ERROR -- Unable to create I/O Completion port! GetLastError(): %d File: %s Line: %d"), GetLastError(), _T( __FILE__ ), __LINE__ );
		DWORD dwError = GetLastError();
		pDirInfo->DeleteSelf( NULL );
		::SetLastError(dwError);//who knows what the last error will be after i call pDirInfo->DeleteSelf(), so set it just to make sure
		return dwError;
	} else {//completion port created/directory associated w/ it successfully
		//if the thread isn't running start it....
		//when the thread starts, it will call ReadDirectoryChangesW and wait 
		//for changes to take place
		if (m_hThread == NULL)
		{
			m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MonitorDirectoryChanges, this, 0, &m_dwThreadID);
			if (m_hThread == NULL)
			{
				TRACE(_T("CDirectoryChangeWatcher::WatchDirectory()-- AfxBeginThread failed!\n"));
				pDirInfo->DeleteSelf(NULL);
				return (GetLastError() == ERROR_SUCCESS)? ERROR_MAX_THRDS_REACHED : GetLastError();
			}
		}

		//thread is running, 
		//signal the thread to issue the initial call to
		//ReadDirectoryChangesW()
		if( m_hThread != NULL )
		{
		   DWORD dwStarted = pDirInfo->StartMonitor(m_hCompPort);

		   if( dwStarted != ERROR_SUCCESS )
		   {//there was a problem!
			   TRACE(_T("Unable to watch directory: %s -- GetLastError(): %d\n"), dwStarted);
			   pDirInfo->DeleteSelf( NULL );
				::SetLastError(dwStarted);//I think this'll set the Err object in a VB app.....
			   return dwStarted;
		   }
		   else
		   {
			   //ReadDirectoryChangesW was successfull!
				//add the directory info to the first empty slot in the array

				//associate the pChangeHandler with this object
				pChangeHandler->ReferencesWatcher(this);//reference is removed when directory is unwatched.
				//CDirWatchInfo::DeleteSelf() must now be called w/ this CDirectoryChangeWatcher pointer becuse
				//of a reference count

				//save the CDirWatchInfo* so I'll be able to use it later.
				VERIFY( AddToWatchInfo(pDirInfo));
				SetLastError(dwStarted);
				return dwStarted;
		   }
		}
		else
		{
			ASSERT(FALSE);//control path shouldn't get here
			::SetLastError(ERROR_MAX_THRDS_REACHED);
			return ERROR_MAX_THRDS_REACHED;
		}
	}
	ASSERT( FALSE );//shouldn't get here.
}

BOOL CDirectoryChangeWatcher::UnwatchAllDirectories()
{
	if (m_hThread != NULL)
	{
		ASSERT(m_hCompPort != NULL);

		CSingleLock lock( &m_csDirWatchInfo, TRUE);
		ASSERT(lock.IsLocked());

		int nMax = m_DirectoriesToWatch.GetSize();
		for(int n = 0; n < nMax; ++n)
		{
			CDirWatchInfo * pDirInfo = m_DirectoriesToWatch[n];
			if (pDirInfo != NULL )
			{
				VERIFY(pDirInfo->UnwatchDirectory(m_hCompPort));

				m_DirectoriesToWatch.SetAt(n, NULL);
				pDirInfo->DeleteSelf(this);
			}

		}
		m_DirectoriesToWatch.RemoveAll();
		//kill off the thread
		PostQueuedCompletionStatus(m_hCompPort, 0, 0, NULL);//The thread will catch this and exit the thread
		//wait for it to exit
		WaitForSingleObject(m_hThread, INFINITE);
		//CloseHandle( m_hThread );//Since thread was started w/ AfxBeginThread() this handle is closed automatically, closing it again will raise an exception
		m_hThread = NULL;
		m_dwThreadID = 0UL;		

		CloseHandle( m_hCompPort );
		m_hCompPort = NULL;
		return TRUE;
	}
	else
	{
#ifdef _DEBUG
		//make sure that there aren't any 
		//CDirWatchInfo objects laying around... they should have all been destroyed 
		//and removed from the array m_DirectoriesToWatch
		if( m_DirectoriesToWatch.GetSize() > 0 )
		{
			for(int i = 0; i < m_DirectoriesToWatch.GetSize(); ++i)
			{
				ASSERT( m_DirectoriesToWatch[i] == NULL );
			}
		}
#endif
	}
	return FALSE;
}


/***************************************************************
FUNCTION:	UnwatchDirectory(const CString & strDirToStopWatching -- if this directory is being watched, the watch is stopped

****************************************************************/
BOOL CDirectoryChangeWatcher::UnwatchDirectory(LPCTSTR lpszDirectoryToStopWatching)
{
	BOOL bRetVal = FALSE;

	if( m_hCompPort != NULL )//The io completion port must be open
	{
		ASSERT(_tcslen(lpszDirectoryToStopWatching) != 0);
		
		CSingleLock lock(&m_csDirWatchInfo, TRUE);
		ASSERT( lock.IsLocked() );	
		int nIdx = -1;
		CDirWatchInfo * pDirInfo = GetDirWatchInfo(lpszDirectoryToStopWatching, nIdx);
		if (pDirInfo != NULL && nIdx != -1)
		{
			//stop watching this directory
			VERIFY( pDirInfo->UnwatchDirectory(m_hCompPort));

			//cleanup the object used to watch the directory
			m_DirectoriesToWatch.SetAt(nIdx, NULL);
			pDirInfo->DeleteSelf(this);
			bRetVal = TRUE;
		}
	}

	return bRetVal;
}

BOOL CDirectoryChangeWatcher::UnwatchDirectory(CDirectoryChangeHandler * pChangeHandler)
/************************************

  This function is called from the dtor of CDirectoryChangeHandler automatically,
  but may also be called by a programmer because it's public...
  
  A single CDirectoryChangeHandler may be used for any number of watched directories.

  Unwatch any directories that may be using this 
  CDirectoryChangeHandler * pChangeHandler to handle changes to a watched directory...
  
  The CDirWatchInfo::m_pChangeHandler member of objects in the m_DirectoriesToWatch
  array will == pChangeHandler if that handler is being used to handle changes to a directory....
************************************/
{
	ASSERT( pChangeHandler );

	CSingleLock lock(&m_csDirWatchInfo, TRUE);
	
	ASSERT( lock.IsLocked() );
	
	int nUnwatched = 0;
	int nIdx = -1;
	CDirWatchInfo * pDirInfo;
	//
	//	go through and unwatch any directory that is 
	//	that is using this pChangeHandler as it's file change notification handler.
	//
	while( (pDirInfo = GetDirWatchInfo( pChangeHandler, nIdx )) != NULL )
	{
		VERIFY( pDirInfo->UnwatchDirectory( m_hCompPort ) );

		nUnwatched++;
		m_DirectoriesToWatch.SetAt(nIdx, NULL);
		pDirInfo->DeleteSelf(this);	
	}
	return (BOOL)(nUnwatched != 0);
}

BOOL CDirectoryChangeWatcher::UnwatchDirectoryBecauseOfError(CDirWatchInfo * pWatchInfo)
//
//	Called in the worker thread in the case that ReadDirectoryChangesW() fails
//	during normal operation. One way to force this to happen is to watch a folder
//	using a UNC path and changing that computer's IP address.
//	
{
	ASSERT( pWatchInfo );
	ASSERT( m_dwThreadID == GetCurrentThreadId() );//this should be called from the worker thread only.
	BOOL bRetVal = FALSE;
	if( pWatchInfo )
	{
		CSingleLock lock(&m_csDirWatchInfo, TRUE);
		
		ASSERT( lock.IsLocked() );
		int nIdx = -1;
		if( GetDirWatchInfo(pWatchInfo, nIdx) == pWatchInfo )
		{
			// we are actually watching this....

			//
			//	Remove this CDirWatchInfo object from the list of watched directories.
			//
			m_DirectoriesToWatch.SetAt(nIdx, NULL);//mark the space as free for the next watch...

			//
			//	and delete it...
			//

			pWatchInfo->DeleteSelf(this);
		
		}

	}
	return bRetVal;
}

//
//	
//	To add the CDirWatchInfo  * to an array.
//	The array is used to keep track of which directories 
//	are being watched.
//
//	Add the ptr to the first non-null slot in the array.
int CDirectoryChangeWatcher::AddToWatchInfo(CDirWatchInfo * pWatchInfo)
{
	CSingleLock lock( &m_csDirWatchInfo, TRUE);
	ASSERT( lock.IsLocked() );
	
	//first try to add it to the first empty slot in m_DirectoriesToWatch
	INT_PTR nMax = m_DirectoriesToWatch.GetSize();
	INT_PTR n = 0;
	for(n = 0; n < nMax; ++n)
	{
		if (m_DirectoriesToWatch[n] == NULL)
		{
			m_DirectoriesToWatch[n] = pWatchInfo;
			break;
		}
	}
	if (n == nMax)
	{
		n = m_DirectoriesToWatch.Add(pWatchInfo);
	}

	return n != -1;
}

UINT CDirectoryChangeWatcher::MonitorDirectoryChanges(LPVOID lParam)
{
    DWORD numBytes;

    CDirWatchInfo* pDirectoryInfo;
    LPOVERLAPPED lpOverlapped;
    
	CDirectoryChangeWatcher* pDirectoryWatcher = reinterpret_cast<CDirectoryChangeWatcher*>(lParam);
	ASSERT(pDirectoryWatcher);

	pDirectoryWatcher->OnThreadInitialize();

    do
    {
		//The io completion request failed...
		//probably because the handle to the directory that
		//was used in a call to ReadDirectoryChangesW has been closed.

		//
		//	calling pdi->CloseDirectoryHandle() will cause GetQueuedCompletionStatus() to return false.
		//  
		//
        if (GetQueuedCompletionStatus(pDirectoryWatcher->m_hCompPort, &numBytes, (PULONG_PTR)&pDirectoryInfo, &lpOverlapped, INFINITE) == FALSE)
		{
			//the directory handle is still open! (we expect this when after we close the directory handle )
			//TRACE(_T("GetQueuedCompletionStatus() returned FALSE\nGetLastError(): %d Completion Key: %p lpOverlapped: %p\n"), GetLastError(), pdi, lpOverlapped);
		}
		
		if (pDirectoryInfo)//pdi will be null if I call PostQueuedCompletionStatus(m_hCompPort, 0,0,NULL);
        {
			//
			//	The following check to AfxIsValidAddress() should go off in the case
			//	that I have deleted this CDirWatchInfo object, but it was still in 
			//	"in the Queue" of the i/o completion port from a previous overlapped operation.
			//
			/***********************************
			The CDirWatchInfo::m_RunningState is pretty much the only member
			of CDirWatchInfo that can be modified from the other thread.
			The functions StartMonitor() and UnwatchDirecotry() are the functions that 
			can modify that variable.

			So that I'm sure that I'm getting the right value, 
			I'm using a critical section to guard against another thread modyfying it when I want
			to read it...
			
			************************************/
			BOOL bObjectShouldBeOk = TRUE;
			try{
			    VERIFY(pDirectoryInfo->LockProperties());//don't give the main thread a chance to change this object
			}
			catch(...){
				//any sort of exception here indicates I've
				//got a hosed object.
				TRACE(_T("CDirectoryChangeWatcher::MonitorDirectoryChanges() -- pdi->LockProperties() raised an exception!\n"));
				bObjectShouldBeOk = FALSE;
			}

			//while we're working with this object...
			if (bObjectShouldBeOk)
			{
				RUNNING_STATE RunState = pDirectoryInfo->GetRunState();
				
				VERIFY(pDirectoryInfo->UnlockProperties());//let another thread back at the properties...
				/***********************************
				 Unlock it so that there isn't a DEADLOCK if 
				 somebody tries to call a function which will 
				 cause CDirWatchInfo::UnwatchDirectory() to be called
				 from within the context of this thread (eg: a function called because of
				 the handler for one of the CDirectoryChangeHandler::On_Filexxx() functions)
		
				************************************/
				ASSERT(pDirectoryInfo->GetChangeHandler());
				switch(RunState)
				{
				case RUNNING_STATE_START_MONITORING:
					{
						//Issue the initial call to ReadDirectoryChangesW()
						if (!ReadDirectoryChangesW(pDirectoryInfo->m_hDirectory,
																		pDirectoryInfo->m_Buffer,//<--FILE_NOTIFY_INFORMATION records are put into this buffer
																		READ_DIR_CHANGE_BUFFER_SIZE,
																		pDirectoryInfo->m_bWatchSubDir,
																		pDirectoryInfo->m_dwChangeFilter,
																		&pDirectoryInfo->m_dwBufLength,//this var not set when using asynchronous mechanisms...
																		&pDirectoryInfo->m_Overlapped,
																		NULL))//no completion routine!
						{
							pDirectoryInfo->m_dwReadDirError = GetLastError();
							if (pDirectoryInfo->GetChangeHandler())
								pDirectoryInfo->GetChangeHandler()->OnWatchStarted(pDirectoryInfo->m_dwReadDirError, pDirectoryInfo->m_strDirName);
						}
						else
						{//read directory changes was successful!
						 //allow it to run normally
							pDirectoryInfo->m_RunningState = RUNNING_STATE_NORMAL;
							pDirectoryInfo->m_dwReadDirError = ERROR_SUCCESS;
							if (pDirectoryInfo->GetChangeHandler())
								pDirectoryInfo->GetChangeHandler()->OnWatchStarted(ERROR_SUCCESS, pDirectoryInfo->m_strDirName);
						}
						//signall that the ReadDirectoryChangesW has been called
						//check CDirWatchInfo::m_dwReadDirError to see whether or not ReadDirectoryChangesW succeeded...
						pDirectoryInfo->m_StartStopEvent.SetEvent();
					}
					break;
				case RUNNING_STATE_STOP:
					{
						//We want to shut down the monitoring of the directory
						//that pdi is managing...
						if (pDirectoryInfo->m_hDirectory != INVALID_HANDLE_VALUE)
						{
							pDirectoryInfo->CloseDirectoryHandle();
							//back up step...GetQueuedCompletionStatus() will still need to return from the last time that ReadDirectoryChangesW() was called.....
							pDirectoryInfo->m_RunningState = RUNNING_STATE_STOP_STEP2;

							pDirectoryInfo->GetChangeHandler()->OnWatchStopped(pDirectoryInfo->m_strDirName);
						}
						else
						{
							//either we weren't watching this direcotry in the first place,
							//or we've already stopped monitoring it....
							pDirectoryInfo->m_StartStopEvent.SetEvent();//set the event that ReadDirectoryChangesW has returned and no further calls to it will be made...
						}					
					}break;
				case RUNNING_STATE_STOP_STEP2:
					{
						//signal that no further calls to ReadDirectoryChangesW will be made
						//and this pdi can be deleted 
						if (pDirectoryInfo->m_hDirectory == INVALID_HANDLE_VALUE)
							pDirectoryInfo->m_StartStopEvent.SetEvent();
						else
							pDirectoryInfo->CloseDirectoryHandle();
					}
					break;
				case RUNNING_STATE_NORMAL:
					{
						if (pDirectoryInfo->GetChangeHandler())
							pDirectoryInfo->GetChangeHandler()->SetChangedDirectoryName(pDirectoryInfo->m_strDirName);
		
						DWORD dwReadBuffer_Offset = 0UL;

						//process the FILE_NOTIFY_INFORMATION records:
						CFileNotifyInformation FileNotify((LPBYTE)pDirectoryInfo->m_Buffer, READ_DIR_CHANGE_BUFFER_SIZE);

						pDirectoryWatcher->ProcessChangeNotifications(FileNotify, pDirectoryInfo, dwReadBuffer_Offset);
						if (!ReadDirectoryChangesW(pDirectoryInfo->m_hDirectory,
											  pDirectoryInfo->m_Buffer + dwReadBuffer_Offset,//<--FILE_NOTIFY_INFORMATION records are put into this buffer 
								              READ_DIR_CHANGE_BUFFER_SIZE - dwReadBuffer_Offset,
											  pDirectoryInfo->m_bWatchSubDir,
										      pDirectoryInfo->m_dwChangeFilter,
											  &pDirectoryInfo->m_dwBufLength,//this var not set when using asynchronous mechanisms...
											 &pDirectoryInfo->m_Overlapped,
											NULL))//no completion routine!
						{
							TRACE(_T("WARNING: ReadDirectoryChangesW has failed during normal operations...failed on directory: %s\n"), pDirectoryInfo->m_strDirName);

							ASSERT(pDirectoryWatcher);
							int nOldThreadPriority = GetThreadPriority(GetCurrentThread());
							SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);							
							
							pDirectoryInfo->m_dwReadDirError = GetLastError();
							pDirectoryInfo->GetChangeHandler()->OnReadDirectoryChangesError(pDirectoryInfo->m_dwReadDirError, pDirectoryInfo->m_strDirName);

							//Do the shutdown
							pDirectoryWatcher->UnwatchDirectoryBecauseOfError(pDirectoryInfo);

							SetThreadPriority(GetCurrentThread(), nOldThreadPriority);
						}
						else
						{
							//success, continue as normal
							pDirectoryInfo->m_dwReadDirError = ERROR_SUCCESS;
						}
					}
					break;
				default:
					TRACE(_T("MonitorDirectoryChanges() -- how did I get here?\n"));
					break;//how did I get here?
				}//end switch( pdi->m_RunningState )
			}//end if( bObjectShouldBeOk )
        }//end if( pdi )
    } while(pDirectoryInfo);

	pDirectoryWatcher->OnThreadExit();
	return 0; //thread is ending
}

//
//	functions for retrieving the directory watch info based on different parameters
//
CDirWatchInfo * CDirectoryChangeWatcher::GetDirWatchInfo(LPCTSTR lpszDirectoryName, int & nRef) const
{
	if (_tcslen(lpszDirectoryName) == 0)// can't be watching a directory if it's you don't pass in the name of it...
		return NULL;		  //
	
	CSingleLock lock(const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);

	INT_PTR nMax = m_DirectoriesToWatch.GetSize();
	CDirWatchInfo* pWatchInfo = NULL;
	for(INT_PTR n = 0; n < nMax; ++n)
	{
		if ((pWatchInfo = m_DirectoriesToWatch[n]) != NULL && pWatchInfo->GetDirectoryName().CompareNoCase(lpszDirectoryName) == 0)
		{
			nRef = n;
			return pWatchInfo;
		}
	}
			
	return NULL;//NOT FOUND
}

CDirWatchInfo * CDirectoryChangeWatcher::GetDirWatchInfo(CDirWatchInfo * pWatchInfo, int & nRef)const
{
	ASSERT( pWatchInfo != NULL );

	CSingleLock lock( const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);
	INT_PTR nMax = m_DirectoriesToWatch.GetSize();
	CDirWatchInfo * pInfo;
	for(INT_PTR n=0; n < nMax; ++n)
	{
		if((pInfo = m_DirectoriesToWatch[n]) != NULL && pInfo == pWatchInfo)
		{
			nRef = n;
			return pInfo;
		}
	}
	return NULL;//NOT FOUND
}

CDirWatchInfo * CDirectoryChangeWatcher::GetDirWatchInfo(CDirectoryChangeHandler * pChangeHandler, int & nRef) const
{
	ASSERT(pChangeHandler != NULL);
	CSingleLock lock( const_cast<CCriticalSection*>(&m_csDirWatchInfo), TRUE);
	INT_PTR nMax = m_DirectoriesToWatch.GetSize();
	CDirWatchInfo * pWatchInfo = NULL;
	for(INT_PTR n=0 ; n < nMax; ++n)
	{
		if ((pWatchInfo = m_DirectoriesToWatch[n]) != NULL && pWatchInfo->GetRealChangeHandler() == pChangeHandler )
		{
			nRef = n;
			return pWatchInfo;
		}
	}
	return NULL;//NOT FOUND
}

/////////////////////////////////////////////////////////////
//
//	Processes the change notifications and dispatches the handling of the 
//	notifications to the CDirectoryChangeHandler object passed to WatchDirectory()
//
/////////////////////////////////////////////////////////////
void CDirectoryChangeWatcher::ProcessChangeNotifications(IN CFileNotifyInformation& FileNotify, 
														 IN CDirWatchInfo * pDirectoryInfo,
														 OUT DWORD & ref_dwReadBuffer_Offset//used in case ...see case for FILE_ACTION_RENAMED_OLD_NAME
														 )
{
	//
	//	Sanity check...
	//	this function should only be called by the worker thread.
	//	
	ASSERT( m_dwThreadID == GetCurrentThreadId() );

	//	Validate parameters...
	//	
	ASSERT(pDirectoryInfo);

	if(!pDirectoryInfo)
	{
		TRACE(_T("Invalid arguments to CDirectoryChangeWatcher::ProcessChangeNotifications() -- pdi is invalid!\n"));
		TRACE(_T("File: %s Line: %d"), _T( __FILE__ ), __LINE__ );
		return;
	}

	DWORD dwLastAction = 0;
	ref_dwReadBuffer_Offset = 0UL;

	CDirectoryChangeHandler * pChangeHandler = pDirectoryInfo->GetChangeHandler();
	//CDelayedDirectoryChangeHandler * pChangeHandler = pdi->GetChangeHandler();
	ASSERT(pChangeHandler);
	//ASSERT( AfxIsValidAddress(pChangeHandler, sizeof(CDelayedDirectoryChangeHandler)) );
	if(pChangeHandler == NULL)
	{
		TRACE(_T("CDirectoryChangeWatcher::ProcessChangeNotifications() Unable to continue, pdi->GetChangeHandler() returned NULL!\n"));
		TRACE(_T("File: %s  Line: %d\n"), _T( __FILE__ ), __LINE__ );
		return;
	}

    do
	{
		switch(FileNotify.GetAction())
		{
		case FILE_ACTION_ADDED:		// a file was added!
				pChangeHandler->OnFileAdded(FileNotify.GetFileNameWithPath(pDirectoryInfo->m_strDirName)); 
			break;
		case FILE_ACTION_REMOVED:	//a file was removed
				pChangeHandler->OnFileRemoved(FileNotify.GetFileNameWithPath(pDirectoryInfo->m_strDirName)); 
			break;
		case FILE_ACTION_MODIFIED:
				pChangeHandler->OnFileModified(FileNotify.GetFileNameWithPath(pDirectoryInfo->m_strDirName)); 
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
			{
				CString strOldFileName = FileNotify.GetFileNameWithPath(pDirectoryInfo->m_strDirName);
				if(FileNotify.GetNextNotifyInformation())
				{
					ASSERT(FileNotify.GetAction() == FILE_ACTION_RENAMED_NEW_NAME );//making sure that the next record after the OLD_NAME record is the NEW_NAME record

					//get the new file name
					CString strNewFileName = FileNotify.GetFileNameWithPath(pDirectoryInfo->m_strDirName);
					pChangeHandler->OnFileNameChanged(strOldFileName, strNewFileName);
				}
				else
				{
					VERIFY(FileNotify.CopyCurrentRecordToBeginningOfBuffer(ref_dwReadBuffer_Offset));
				}
				break;
			}
		case FILE_ACTION_RENAMED_NEW_NAME:
			{
				ASSERT(dwLastAction == FILE_ACTION_RENAMED_OLD_NAME );
				ASSERT( FALSE );//this shouldn't get here
			}
		default:
			TRACE(_T("CDirectoryChangeWatcher::ProcessChangeNotifications() -- unknown FILE_ACTION_ value! : %d\n"), FileNotify.GetAction() );
			break;//unknown action
		}
		dwLastAction = FileNotify.GetAction();
	} while(FileNotify.GetNextNotifyInformation());
}

LONG CDirectoryChangeWatcher::ReleaseReferenceToWatcher(CDirectoryChangeHandler * pChangeHandler)
{
	ASSERT(pChangeHandler);
	return pChangeHandler->ReleaseReferenceToWatcher(this);
}