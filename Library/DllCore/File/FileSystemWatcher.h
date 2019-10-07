#pragma once

/*******************************************************************************
 *  @file         FileSystemWatcher.h 2016\10\11 10:56:00 $
 *  @author    剑客<1054457048@qq.com>
 *  @brief       监控文件或目录变化，目前只能做审计功能，不能拦截
 ******************************************************************************/

typedef enum tagNotifyFilter
{
	FILTER_FILE_NAME					= 0x00000001, // add/remove/rename
	FILTER_DIR_NAME					= 0x00000002, // add/remove/rename
	FILTER_ATTR_NAME				= 0x00000004,
	FILTER_SIZE_NAME					= 0x00000008,
	FILTER_LAST_WRITE_NAME	= 0x00000010, // timestamp
	FILTER_LAST_ACCESS_NAME	= 0x00000020, // timestamp
	FILTER_CREATION_NAME		= 0x00000040, // timestamp
	FILTER_SECURITY_NAME		= 0x00000100,
}NOTIFY_FILTER;

typedef enum tagAction
{
	ACTION_ERRSTOP					= -1,
	ACTION_ADDED						= 0x00000001,
	ACTION_REMOVED					= 0x00000002,
	ACTION_MODIFIED					= 0x00000003,
	ACTION_RENAMED_OLD		= 0x00000004,
	ACTION_RENAMED_NEW      = 0x00000005
}FILE_ACTION;

typedef void (CALLBACK *LPDEALFUNCTION)(FILE_ACTION Action, LPCTSTR lpszFileName, LPVOID lParam);

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CFileSystemWatcher
{
public:
	CFileSystemWatcher();
	~CFileSystemWatcher();

public:
	BOOL Run(LPCTSTR lpszMonitorDir, BOOL bWatchSubtree, DWORD dwNotifyFilter, LPDEALFUNCTION FnDeal, LPVOID lParam);
	void Close(DWORD dwMilliseconds = INFINITE );

protected:
	static DWORD WINAPI Routine(LPVOID lParam);

	static UINT CALLBACK DispathMessage(LPVOID lParam);
private:
	HANDLE m_hFile;
	HANDLE m_hThread;

	DWORD	m_dwNotifyFilter;
	BOOL		m_bWatchSubtree;
	BOOL		m_bRequestStop;
	CString		m_strMonitorPath;

	LPVOID	m_DealFunParam;
	LPDEALFUNCTION m_DealFun;
};