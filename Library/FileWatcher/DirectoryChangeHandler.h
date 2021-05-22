#pragma once

// CDirectoryChangeWatcher and CDirectoryChangeHandler classes.
//
//  Uses an io completion port and ReadDirectoryChangesW -- this code will only work on 
//	Windows NT/2000/XP.
//
//	The directory being watched must be a directory on a Windows NT/2000/XP machine
//
//
//	These classes are based on the FWatch sample program in the SDK.
//
//
//	If you get a compile time error that ReadDirectoryChangesW is an undeclared identifier,
//  you'll need to #define _WIN32_WINNT 0x400 in stdafx.h.

/***********************************
 A class to handle changes to files in a directory.  
 The virtual On_Filexxx() functions are called whenever changes are made to a watched directory that is being handled by this object...
 The On_Filexxx() functions execute in the context of the main thread if true is passed to the constructor of CDirectoryChangeWatcher,
 OR they fire in the context of a worker thread if false is passed to the constructor of CDirectoryChangeWatcher

  NOTES: 
		A CDirectoryChangeHandler can only be used by ONE CDirectoryChangeWatcher object,
		but a CDirectoryChangeWatcher object may use multiple CDirectoryChangeHandler objects.

	When this object is destroyed, whatever directories that it was being used to handle directory changes for
	will automatically be 'unwatched'.

	The class is reference counted.  The reference count is increased every time it is used 
	in a (successfull) call to CDirectoryChangeWatcher::WatchDirectory() and is decremented whenever
	the directory becomes 'unwatched'.

   The only notifications are File Added, Removed, Modified, and Renamed.
   Even though the CDirectoryChangeWatcher::WatchDirectory (which'll call ReadDirectoryChangesW()) function allows you to specify flags 
   to watch for changes to last access time, last write time, attributes changed, etc,
   these changes all map out to On_FileModified() which doesn't specify the type of modification.


  NOTE:   The CDirectoryChangeHandler::On_Filexxx() functions
		  are called in the context of the main thread, the thread that called CDirectoryChangeWatcher::WatchDirectory(),
		  if you pass true to the constructor of CDirectoryChangeWatcher. This is accomplished via a hidden window,
		  and REQUIRES that your app has a message pump. 
		  For console applications, or applications w/out a message pump, you can pass false to the constructor
		  of CDirectoryChangeWatcher, and these notifications will fire in the context of a worker thread.  By passing false
		  to the constructor of CDirectoryChangeWatcher, you do NOT NEED a message pump in your application.



************************************/

class CDirectoryChangeWatcher;
class CDirectoryChangeHandler : public IDirectoryChangeHandler
{
public:
	CDirectoryChangeHandler();
	virtual ~CDirectoryChangeHandler();

public:
	//this class is reference counted
	LONG AddRef();
	LONG Release();
	LONG GetRef() const;

public:
	BOOL UnwatchDirectory();//causes CDirectoryChangeWatcher::UnwatchDirectory() to be called.

protected:
	virtual void OnFileAdded(LPCTSTR lpszFileName);
	virtual void OnFileModified(LPCTSTR lpszFileName);
	virtual void OnFileRemoved(LPCTSTR lpszFileName);
	virtual void OnFileNameChanged(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName);
	virtual void OnReadDirectoryChangesError(DWORD dwError, LPCTSTR lpszNewFileName);

	virtual void OnWatchStarted(DWORD dwError, LPCTSTR lpszDirectoryName);
	virtual void OnWatchStopped(LPCTSTR lpszDirectoryName);
	virtual BOOL OnFilterNotification(DWORD dwNotifyAction, LPCTSTR szFileName, LPCTSTR szNewFileName);

	void SetChangedDirectoryName(const CString & strChangedDirName);
	const CString& GetChangedDirectoryName() const;
private:
	LONG  ReferencesWatcher(CDirectoryChangeWatcher * pDirChangeWatcher);
	LONG  ReleaseReferenceToWatcher(CDirectoryChangeWatcher * pDirChangeWatcher);

private:
	LONG m_nRefCnt;

	CString m_strChangedDirectoryName;//will be removed in a future release.

	friend class CDirectoryChangeWatcher;
	friend class CDelayedDirectoryChangeHandler;

	CDirectoryChangeWatcher * m_pDirChangeWatcher;
	LONG   m_nWatcherRefCnt; //<-- when this reaches 0, m_pDirChangeWatcher is set to NULL
	CCriticalSection m_csWatcher;
};