/*******************************************************************
	The classes in this file implement methods to ensure that file change 
	notifications are fired in a thread other than the worker thread used
	by CDirectoryChangeWatcher.

	Dispatching the notifications in to a different thread improves the performance
	of CDirectoryChangeWatcher so that it can process more notifications faster
	and notifications aren't 'lost'.


	There are two methods of dispatching functions to another thread.

		1)  One is to use the message pump associated w/ the main thread by posting notifications
			to a hidden window. This is implemented w/ the class CDelayedNotificationWindow.

		2)	The other is to create a worker thread that implements a message pump.  This is 
			implemented w/ the class CDelayedNotificationThread.


	If your app uses a GUI then it has a already has message pump.  
	You can make sure that CDelayedNotificationWindow is used in this case.
	The advantage here being that there is one less worker thread used in your program.

	If your app is a command line app or otherwise doesn't have a GUI,
	then you will want to make sure that you are using the CDelayedNotificationThread
	to dispatch notifications to another thread.

	This is determined by a flag passed is passed to the constructor of CDirecotryChangeWatcher

********************************************************************/

//
//	Abstract base class for ensuring notifications are fired in a thread 
//
//

#pragma once


typedef enum eFunctionToDispatch
{	
	eFunctionNotDefined = -1,
	eOn_FileAdded				= FILE_ACTION_ADDED, 
	eOn_FileRemoved			= FILE_ACTION_REMOVED, 
	eOn_FileModified			= FILE_ACTION_MODIFIED,
	eOn_FileNameChanged	= FILE_ACTION_RENAMED_OLD_NAME,
	eOn_ReadDirectoryChangesError,
	eOn_WatchStarted,
	eOn_WatchStopped
}FUNCTION_TO_DISPATCH;

//
//	 A class to help dispatch the change notifications to the main thread.
//
//	 This class holds the data in memory until the notification can be dispatched.(ie: this is the time between when the notification is posted, and the clients notification code is called).
//
//
class CDirChangeNotification
{
public:
	explicit CDirChangeNotification(CDelayedDirectoryChangeHandler * pDelayedHandler, DWORD dwPartialPathOffset);
	~CDirChangeNotification();
private:
	CDirChangeNotification();//not implemented
public:
	void PostOn_FileAdded(LPCTSTR szFileName);
	void PostOn_FileRemoved(LPCTSTR szFileName);
	void PostOn_FileNameChanged(LPCTSTR szOldName, LPCTSTR szNewName);
	void PostOn_FileModified(LPCTSTR szFileName);
	void PostOn_ReadDirectoryChangesError(DWORD dwError, LPCTSTR szDirectoryName);
	void PostOn_WatchStarted(DWORD dwError, LPCTSTR szDirectoryName);
	void PostOn_WatchStopped(LPCTSTR szDirectoryName);

	void DispatchNotificationFunction();
protected:
	void PostNotification();
	
private:
	friend class CDelayedDirectoryChangeHandler;
	CDelayedDirectoryChangeHandler * m_pDelayedHandler;

	eFunctionToDispatch m_eFunctionToDispatch;
	//Notification Data:
	TCHAR *	m_szFileName1;//<-- is the szFileName parameter to On_FileAdded(),On_FileRemoved,On_FileModified(), and is szOldFileName to On_FileNameChanged(). Is also strDirectoryName to On_ReadDirectoryChangesError(), On_WatchStarted(), and On_WatchStopped()
	TCHAR *	m_szFileName2;//<-- is the szNewFileName parameter to On_FileNameChanged()
	DWORD m_dwError;	  //<-- is the dwError parameter to On_WatchStarted(), and On_ReadDirectoryChangesError()
	//

	DWORD m_dwPartialPathOffset;//helps support FILTERS_CHECK_PARTIAL_PATH...not passed to any functions other than may be used during tests in CDelayedDirectoryChangeHandler::NotifyClientOfFileChange()

	friend class CDirChangeNotification;
	friend class CDirectoryChangeWatcher;
	friend DWORD GetPathOffsetBasedOnFilterFlags(CDirChangeNotification*,DWORD);//a friend function
};