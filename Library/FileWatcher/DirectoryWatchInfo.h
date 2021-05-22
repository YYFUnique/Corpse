#pragma once

typedef enum tagRUNNING_STATE
{
	RUNNING_STATE_NOT_SET						= 0,
	RUNNING_STATE_START_MONITORING	= 1,
	RUNNING_STATE_STOP								= 2,
	RUNNING_STATE_STOP_STEP2					= 3,
	RUNNING_STATE_STOPPED						= 4,
	RUNNING_STATE_NORMAL						= 5,
}RUNNING_STATE;

class CDirectoryChangeHandler;
class CDirectoryChangeWatcher;
class CDelayedDirectoryChangeHandler;
//this class is used internally by CDirectoryChangeWatcher
//to help manage the watched directories
class CDirWatchInfo 
{
private:
	CDirWatchInfo();		//private & not implemented
	CDirWatchInfo & operator=(const CDirWatchInfo & rhs);//so that they're aren't accidentally used. -- you'll get a linker error
public:
	CDirWatchInfo(HANDLE hDirecoty, LPCTSTR lpszDirectoryName, CDirectoryChangeHandler * pChangeHandler, 
							DWORD dwChangeFilter, BOOL bWatchSubDir, BOOL bAppHasGUI, 
							LPCTSTR szIncludeFilter,LPCTSTR szExcludeFilter, DWORD dwFilterFlags);
private:
	~CDirWatchInfo();//only I can delete myself....use DeleteSelf()
public:
	void DeleteSelf(CDirectoryChangeWatcher * pWatcher);

	DWORD StartMonitor(HANDLE hCompPort);
	BOOL UnwatchDirectory(HANDLE hCompPort);

	DWORD GetReadDirectoryStatus() const;
	void SetReadDirectoryStatus(DWORD dwReadDirectoryStatus = 0);

	CString& GetDirectoryName();

	RUNNING_STATE GetRunState() const;
	HANDLE GetDirectoryHANDLE() const;
protected:
	BOOL SignalShutdown( HANDLE hCompPort );
	BOOL WaitForShutdown();			   
public:
	BOOL LockProperties() { return m_cs.Lock(); }
	BOOL UnlockProperties(){ return m_cs.Unlock();}

	CDelayedDirectoryChangeHandler* GetChangeHandler() const;
	CDirectoryChangeHandler * GetRealChangeHandler() const;//the 'real' change handler is your CDirectoryChangeHandler derived class.
	CDirectoryChangeHandler * SetRealDirectoryChangeHandler(CDirectoryChangeHandler * pChangeHandler);

	BOOL CloseDirectoryHandle();
	
private:
	//CDirectoryChangeHandler * m_pChangeHandler;
	HANDLE      m_hDirectory;				//handle to directory that we're watching
	DWORD		m_dwChangeFilter;
	BOOL			m_bWatchSubDir;
	CString			m_strDirName;			//name of the directory that we're watching
	CHAR			m_Buffer[ READ_DIR_CHANGE_BUFFER_SIZE ];//buffer for ReadDirectoryChangesW
	DWORD       m_dwBufLength;			//length or returned data from ReadDirectoryChangesW -- ignored?...
	DWORD		m_dwReadDirError;		//indicates the success of the call to ReadDirectoryChanges()
	OVERLAPPED  m_Overlapped;
	CCriticalSection m_cs;
	CEvent		m_StartStopEvent;

	RUNNING_STATE m_RunningState;
	CDelayedDirectoryChangeHandler * m_pChangeHandler;
	friend class CDirectoryChangeWatcher;
};//end nested class CDirWatchInfo 