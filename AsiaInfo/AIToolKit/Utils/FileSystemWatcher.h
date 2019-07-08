#pragma once

typedef enum tagFilter
{
	FILTER_FILE_NAME					= 0x00000001,
	FILTER_DIR_NAME					= 0x00000002,
	FILTER_ATTR_NAME				=	0x00000004,
	FILTER_SIZE_NAME					= 0x00000008,
	FILTER_LAST_WRITE_NAME	= 0x00000010,
	FILTER_LAST_ACCESS_NAME = 0x00000020,
	FILTER_CREATION_NAME		=	0x00000040,
	FILTER_SECURITY_NAME		=	0x00000080,
}FILTER;

typedef enum tagACTION
{
	ACTION_ERRSTOP					=	-1,
	ACTION_ADDED						=	0x00000001,
	ACTION_REMOVED					= 0x00000002,
	ACTION_MODIFIED					=	0x00000003,
	ACTION_RENAMED_OLD		=	0x00000004,
	ACTION_RENAMED_NEW		=	0x00000005,
}ACTION;

typedef void (__stdcall *LPDEALFUNCTION)(ACTION act, LPCTSTR lpszFileName, LPVOID lParam);

class CFileWatcher
{
public:
	CFileWatcher();
	~CFileWatcher();

public:
	BOOL Run(LPCTSTR lpszDir, BOOL bwatchSubTree, DWORD dwNotifyFilter, LPDEALFUNCTION lpDealFun, LPVOID lParam);
	void Close(DWORD dwMillisecondes = INFINITE);
protected:
	static DWORD WINAPI ThreadProc(LPVOID lParam);
private:
	HANDLE	m_hDir;
	HANDLE	m_hThread;
	volatile  BOOL m_bRequestStop;
	LPDEALFUNCTION m_pDealFun;
	CString		m_strMonitorPath;
	LPVOID	m_pDealFunParam;
	DWORD	m_dwNOtifyFilter;
	BOOL		m_bWatchSubTree;

};