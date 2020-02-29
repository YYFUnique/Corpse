#pragma once

//////////////////////////////////////////////////////////////////////////
//
//	This class makes it so that a file change notification is executed in the
//	context of the main thread, and not the worker thread.
//
//
//	It works by creating a hidden window.  When it receieves a notification
//	via one of the On_Filexxx() functions, a message is posted to this window.
//	when the message is handled, the notification is fired again in the context
//	of the main thread, or whichever thread that called CDirectoryChangeWatcher::WatchDirectory()
//
//
/////////////////////////////////////////////////////////////////////////////
//	Note this code wants to use PathMatchSpec()
//	which is only supported on WINNT 4.0 w/ Internet Explorer 4.0 and above.
//	PathMatchSpec is fully supported on Win2000/XP.
//
//	For the case of WINNT 4.0 w/out IE 4.0, we'll use a simpler function.
//	some functionality is lost, but such is the price.
//

typedef BOOL (STDAPICALLTYPE * FUNC_PatternMatchSpec)(LPCTSTR pszFile, LPCTSTR pszSpec);

//
//	Decorates an instance of a CDirectoryChangeHandler object.
//	Intercepts notification function calls and posts them to 
//	another thread through a method implemented by a class derived from 
//	CDelayedNotifier
//	
//
//	This class implements dispatching the notifications to a thread
//	other than CDirectoryChangeWatcher::MonitorDirectoryChanges()
//
//	Also supports the include and exclude filters for each directory
//
class CDelayedDirectoryChangeHandler : public CDirectoryChangeHandler
{
public:
	CDelayedDirectoryChangeHandler(CDirectoryChangeHandler * pRealHandler, BOOL bAppHasGUI, LPCTSTR szIncludeFilter, LPCTSTR szExcludeFilter, DWORD dwFilterFlags);
	virtual ~CDelayedDirectoryChangeHandler();
private:
	CDelayedDirectoryChangeHandler();//not implemented.
public:
	CDirectoryChangeHandler * GetRealChangeHandler()const { return m_pRealHandler; }
	CDirectoryChangeHandler * & GetRealChangeHandler(){ return m_pRealHandler; }//FYI: PCLint will give a warning that this exposes a private/protected member& defeats encapsulation.  

	void PostNotification(CDirChangeNotification * pNotification);
	void DispatchNotificationFunction(CDirChangeNotification * pNotification);

protected:
	//These functions are called when the directory to watch has had a change made to it
	void OnFileAdded(LPCTSTR lpszFileName);
	void OnFileRemoved(LPCTSTR lpszFileName);
	void OnFileModified(LPCTSTR lpszFileName);
	void OnFileNameChanged(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName);
	void OnReadDirectoryChangesError(DWORD dwError, LPCTSTR lpszNewFileName);

	void OnWatchStarted(DWORD dwError, const CString & strDirectoryName);
	void OnWatchStopped(const CString & strDirectoryName);
	
	void SetChangedDirectoryName(const CString & strChangedDirName);
	const CString & GetChangedDirectoryName()const;

	BOOL WaitForOnWatchStoppedDispatched();//see comments in .cpp

	BOOL NotifyClientOfFileChange(CDirChangeNotification * pNot);

	BOOL IncludeThisNotification(LPCTSTR szFileName);	//	based on file name.
	BOOL ExcludeThisNotification(LPCTSTR szFileName);	//	Allows us to filter notifications
														//
	
	CDirChangeNotification * GetNotificationObject();
	void DisposeOfNotification(CDirChangeNotification * pNotification);

	CDelayedNotifier * m_pDelayNotifier;
	CDirectoryChangeHandler * m_pRealHandler;	

	// m_bAppHasGUI: 
	//   This flag, if set to true, indicates that the app has a message
	//	 pump, and that functions are dispatched to the main thread.
	BOOL m_bAppHasGUI;	
	//   Otherwise, functions are dispatched to a separate worker thread.
	//
	DWORD m_dwFilterFlags;

	DWORD m_dwPartialPathOffset; //helps support FILTERS_CHECK_PARTIAL_PATH
	void SetPartialPathOffset(const CString & strWatchedDirName);
private:
	BOOL _PathMatchSpec(LPCTSTR szPath, LPCTSTR szPattern);
	BOOL InitializePathMatchFunc(LPCTSTR szIncludeFilter, LPCTSTR szExcludeFilter);
	BOOL InitializePatterns(LPCTSTR szIncludeFilter, LPCTSTR szExcludeFilter);
	void UninitializePathMatchFunc();

	BOOL UsesRealPathMatchSpec() const;//are we using PathMatchSpec() or wildcmp()?
private:
	HANDLE m_hWatchStoppedDispatchedEvent;//supports WaitForOnWatchStoppedDispatched()

	TCHAR * m_szIncludeFilter;		//	Supports the include
	TCHAR * m_szExcludeFilter;		//	& exclude filters

	//
	//	Load PathMatchSpec dynamically because it's only supported if IE 4.0 or greater is
	//	installed.
	static HMODULE s_hShlwapi_dll;//for the PathMatchSpec() function
	static BOOL s_bShlwapi_dllExists;//if on NT4.0 w/out IE 4.0 or greater, this'll be false
	static long s_nRefCnt_hShlwapi;
	static FUNC_PatternMatchSpec s_fpPatternMatchSpec;

	friend class CDirWatchInfo;
	friend class CDirectoryChangeWatcher;
	//note: if the PathMatchSpec function isn't found, wildcmp() is used instead.
	//
	//	to support multiple file specs separated by a semi-colon,
	//	the include and exclude filters that are passed into the 
	//	the constructor are parsed into separate strings
	//	which are all checked in a loop.
	//
	int m_nNumIncludeFilterSpecs;
	int m_nNumExcludeFilterSpecs;
};

