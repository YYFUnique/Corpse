#pragma once
#if TRUE
#else
#include <map>
#include <vector>

typedef std::map<CString, int> CFileModifyMap;

typedef std::vector<LPCTSTR> CPathCollection;

class CDirectoryChangeHandlerEx : public CDirectoryChangeHandler
{
public:
	CDirectoryChangeHandlerEx();
protected:
	//These functions are called when the directory to watch has had a change made to it
	virtual void OnFileAdded(LPCTSTR lpszFileName);
	virtual void OnFileModified(LPCTSTR lpszFileName);
	virtual void OnFileRemoved(LPCTSTR lpszFileName);
	virtual void OnFileNameChanged(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName);

	virtual void OnWatchStarted(DWORD dwError, LPCTSTR lpszDirectoryName);
	virtual void OnWatchStopped(LPCTSTR lpszDirectoryName);
protected:
	static UINT CALLBACK ThreadProc(LPVOID lParam);
protected:
	CFileModifyMap m_MapFileModify;
	CCriticalSection m_csHandler;
};
#endif