#pragma once

class IDirectoryChangeHandler
{
	//
	//	OnFileAdded()
	//
	//	This function is called when a file in one of the watched folders(or subfolders)
	//	has been created.
	//
	//	For this function to execute you'll need to specify FILE_NOTIFY_CHANGE_FILE_NAME or FILE_NOTIFY_CHANGE_DIR_NAME(for directories)
	//  when you call CDirectoryChangeWatcher::WatchDirectory()
	//
	virtual void OnFileAdded(LPCTSTR lpszFileName) = 0;

	//
	//	OnFileRemoved()
	//
	//	This function is called when a file in one of the watched folders(or subfolders)
	//	has been deleted(or moved to another directory)
	//
	//	For this function to execute you'll need to specify FILE_NOTIFY_CHANGE_FILE_NAME or FILE_NOTIFY_CHANGE_DIR_NAME(for directories)
	//  when you call CDirectoryChangeWatcher::WatchDirecotry()
	//
	virtual void OnFileRemoved(LPCTSTR lpszFileName) = 0;
	
	//
	//	OnFileNameChanged()
	//
	//	This function is called when a file in one of the watched folders(or subfolders)
	//	has been renamed.
	//
	//
	//	You'll need to specify FILE_NOTIFY_CHANGE_FILE_NAME (or FILE_NOTIFY_CHANGE_DIR_NAME(for directories))
	//	when you call CDirectoryChangeWatcher::WatchDirectory()
	//
	//	
	virtual void OnFileNameChanged(LPCTSTR  lpszOldFileName, LPCTSTR  lpszNewFileName) = 0;

	//
	//	OnFileModified()
	//
	//	This function is called whenever an attribute specified by the watch
	//	filter has changed on a file in the watched directory or 
	//
	//	Specify any of the following flags when you call CDirectoryChangeWatcher::WatchDirectory()
	//  
	//
	//	FILE_NOTIFY_CHANGE_ATTRIBUTES
	//	FILE_NOTIFY_CHANGE_SIZE 
	//	FILE_NOTIFY_CHANGE_LAST_WRITE 
	//	FILE_NOTIFY_CHANGE_LAST_ACCESS
	//	FILE_NOTIFY_CHANGE_CREATION (* See Note # 1* )
	//	FILE_NOTIFY_CHANGE_SECURITY
	//
	//	
	//	General Note)  Windows tries to optimize some of these notifications.  You may not get 
	//				   a notification every single time a file is accessed for example.  
	//				   There's a MS KB article or something on this(sorry forgot which one).
	//
	//	Note #1	)   This notification isn't what you might think(FILE_NOTIFY_CHANGE_CREATION). 
	//				See the help files for ReadDirectoryChangesW...
	//				This notifies you of a change to the file's 
	//				creation time, not when the file is created.  
	//				Use FILE_NOTIFY_CHANGE_FILE_NAME to know about newly created files.
	//
	virtual void OnFileModified(LPCTSTR lpszFileName) = 0;
	
	//
	//	OnReadDirectoryChangesError()
	//
	//	This function is called when ReadDirectoryChangesW() fails during normal
	//	operation (ie: some time after you've called CDirectoryChangeWatcher::WatchDirectory())
	//
	//
	//	NOTE:  *** READ THIS *** READ THIS *** READ THIS *** READ THIS ***
	//
	//	NOTE: If this function has been called, the watched directory has been automatically unwatched.
	//			You will not receive any further notifications for that directory until 
	//			you call CDirectoryChangeWatcher::WatchDirectory() again.
	//
	//	On_WatchStopped() will not be called.
	virtual void OnReadDirectoryChangesError(DWORD dwError, LPCTSTR lpszDirectoryName) = 0;
	
	//
	//	void OnWatchStarted()
	//
	//	This function is called when a directory watch has begun.  
	//	It will be called whether CDirectoryChangeWatcher::WatchDirectory() is successful or not. Check the dwError parameter.
	//
	//	PARAMETERS:
	//	DWORD dwError					 -- 0 if successful, else it's the return value of GetLastError() 
	//										indicating why the watch failed.
	//	const CString & strDirectoryName -- The full path and name of the directory being watched.
	virtual void OnWatchStarted(DWORD dwError, LPCTSTR lpszDirectoryName) = 0;
	
	//
	//	void OnWatchStopped()
	//
	//	This function is called when a directory is unwatched (except on the case of the direcotry being unwatched due to an error)
	//
	//	WARNING:  *** READ THIS *** READ THIS *** READ THIS *** READ THIS ***
	//
	//	This function MAY be called before the destructor of CDirectoryChangeWatcher 
	//	finishes.  
	//
	//	Be careful if your implementation of this fuction
	//	interacts with some sort of a window handle or other object(a class, a file, etc.).  
	//	It's possible that that object/window handle will NOT be valid anymore the very last time
	//	that On_WatchStopped() is called.  
	//	This scenario is likely if the CDirectoryChangeWatcher instance is currently watching a
	//	directory, and it's destructor is called some time AFTER these objects/windows
	//	your change handler interacts with have been destroyed.
	//	
	//	If your CDirectoryChangeHandler derived class interacts w/ a window or other
	//	object, it's a good idea to unwatch any directories before the object/window is destroyed.
	//	Otherwise, place tests for valid objects/windows in the implementation of this function.
	//
	//  Failure to follow either tip can result in a mysterious RTFM error, or a 'Run time errors'
	//
	virtual void OnWatchStopped(LPCTSTR lpszDirectoryName) = 0;

	//
	//	bool On_FilterNotification(DWORD dwNotifyAction, LPCTSTR szFileName, LPCTSTR szNewFileName);
	//
	//	This function gives your class a chance to filter unwanted notifications.
	//
	//	PARAMETERS: 
	//			DWORD	dwNotifyAction	-- specifies the event to filter
	//			LPCTSTR szFileName		-- specifies the name of the file for the event.
	//			LPCTSTR szNewFileName	-- specifies the new file name of a file that has been renamed.
	//
	//			**	szFileName and szNewFileName will always be the full path and file name with extention.
	//
	//	RETURN VALUE:
	//			return true , and you will receive the notification.
	//			return false, and your class will NOT receive the notification.
	//
	//	Valid values of dwNotifyAction:
	//		FILE_ACTION_ADDED			-- On_FileAdded() is about to be called.
	//		FILE_ACTION_REMOVED			-- On_FileRemoved() is about to be called.
	//		FILE_ACTION_MODIFIED		-- On_FileModified() is about to be called.
	//		FILE_ACTION_RENAMED_OLD_NAME-- On_FileNameChanged() is about to be call.
	//
	//	  
	//	NOTE:  When the value of dwNotifyAction is FILE_ACTION_RENAMED_OLD_NAME,
	//			szFileName will be the old name of the file, and szNewFileName will
	//			be the new name of the renamed file.
	//
	//  The default implementation always returns true, indicating that all notifications will 
	//	be sent.
	//
	//	NOTE:	This function may or may not be called depending upon the flags you specify to control
	//			filter behavior.
	//			If you are specifying filters when watching the directory, you will not get this notification
	//			if the file name does not pass the filter test, even if this function returns true.
	//

	//
	//
	//	End Override these functions (ie: don't worry about the rest of this class)
	//
	virtual BOOL OnFilterNotification(DWORD dwNotifyAction, LPCTSTR szFileName, LPCTSTR szNewFileName) = 0;
};