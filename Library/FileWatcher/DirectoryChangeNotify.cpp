#include "StdAfx.h"
#include "DirectoryChangeNotify.h"

//
//	This is the wndproc for the notification window
//
//	it's here to dispatch the notifications to the client
//
//static LRESULT CALLBACK DelayedNotificationWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	if (uMsg == UWM_DELAYED_DIRECTORY_NOTIFICATION)
//	{
//		CDirChangeNotification * pNotification = reinterpret_cast<CDirChangeNotification*>(lParam);
//		ASSERT(  pNotification );
//		if( pNotification )
//		{
//			DWORD dwEx(0);
//			__try{
//				pNotification->DispatchNotificationFunction();
//			}
//			__except(dwEx = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER){
//				//An exception was raised:
//				//
//				//	Likely cause: there was a problem creating the CDelayedDirectoryChangeHandler::m_hWatchStoppedDispatchedEvent object
//				//	and the change handler object was deleted before the notification could be dispatched to this function.
//				//
//				//  or perhaps, somebody's implementation of an overridden function caused an exception
//				TRACE(_T("Following exception occurred: %d -- File: %s Line: %d\n"), dwEx, _T(__FILE__), __LINE__);
//			}
//		}
//
//		return 0UL;
//	}
//	else
//		return DefWindowProc(hWnd, uMsg, wParam, lParam);
//}

//////////////////////////////////////////////////////////////////////////
//
//CDirChangeNotification member functions:
//
CDirChangeNotification::CDirChangeNotification(CDelayedDirectoryChangeHandler *	pDelayedHandler, DWORD dwPartialPathOffset)
	:m_pDelayedHandler( pDelayedHandler )
	,m_szFileName1(NULL)
	,m_szFileName2(NULL)
	,m_dwError(0UL)
	,m_dwPartialPathOffset(dwPartialPathOffset)
{
	ASSERT( pDelayedHandler );
}

CDirChangeNotification::~CDirChangeNotification()
{
	if( m_szFileName1 ) free(m_szFileName1), m_szFileName1 = NULL;
	if( m_szFileName2 ) free(m_szFileName2), m_szFileName2 = NULL;
}

void CDirChangeNotification::DispatchNotificationFunction()
{
	ASSERT( m_pDelayedHandler );
	if( m_pDelayedHandler )
		m_pDelayedHandler->DispatchNotificationFunction( this );
}

void CDirChangeNotification::PostOn_FileAdded(LPCTSTR szFileName)
{
	ASSERT( szFileName );
	m_eFunctionToDispatch	= eOn_FileAdded;
	m_szFileName1			= _tcsdup( szFileName) ;
	//
	// post the message so it'll be dispatch by another thread.
	PostNotification();

}
void CDirChangeNotification::PostOn_FileRemoved(LPCTSTR szFileName)
{
	ASSERT( szFileName );
	m_eFunctionToDispatch	= eOn_FileRemoved;
	m_szFileName1			= _tcsdup( szFileName) ;
	//
	// post the message so it'll be dispatched by another thread.
	PostNotification();

}
void CDirChangeNotification::PostOn_FileNameChanged(LPCTSTR szOldName, LPCTSTR szNewName)
{
	ASSERT( szOldName && szNewName );

	m_eFunctionToDispatch	= eOn_FileNameChanged;
	m_szFileName1			= _tcsdup( szOldName) ;
	m_szFileName2			= _tcsdup( szNewName) ;
	//
	// post the message so it'll be dispatched by another thread.
	PostNotification();

}

void CDirChangeNotification::PostOn_FileModified(LPCTSTR szFileName)
{
	ASSERT( szFileName );

	m_eFunctionToDispatch	= eOn_FileModified;
	m_szFileName1			= _tcsdup( szFileName );
	//
	// post the message so it'll be dispatched by another thread.
	PostNotification();
}

void CDirChangeNotification::PostOn_ReadDirectoryChangesError(DWORD dwError, LPCTSTR szDirectoryName)
{
	ASSERT( szDirectoryName );

	m_eFunctionToDispatch = eOn_ReadDirectoryChangesError;
	m_dwError			  = dwError;
	m_szFileName1		  = _tcsdup(szDirectoryName);
	//
	// post the message so it'll be dispatched by the another thread.
	PostNotification();

}

void CDirChangeNotification::PostOn_WatchStarted(DWORD dwError, LPCTSTR szDirectoryName)
{
	ASSERT( szDirectoryName );

	m_eFunctionToDispatch = eOn_WatchStarted;
	m_dwError			  =	dwError;
	m_szFileName1		  = _tcsdup(szDirectoryName);

	PostNotification();
}

void CDirChangeNotification::PostOn_WatchStopped(LPCTSTR szDirectoryName)
{
	ASSERT( szDirectoryName );

	m_eFunctionToDispatch = eOn_WatchStopped;
	m_szFileName1		  = _tcsdup(szDirectoryName);

	PostNotification();
}

void CDirChangeNotification::PostNotification()
{
	ASSERT(m_pDelayedHandler);
	if(m_pDelayedHandler)
		m_pDelayedHandler->PostNotification(this);
}