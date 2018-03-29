#pragma once

#include "DllCore/Utils/DuiList.h"

#ifdef LS_STATIC_LIB_CALL
#define DEAMON_API __declspec(dllexport)
#elif defined  DEAMON_EXPORTS
#define DEAMON_API __declspec(dllexport)
#else
#define DEAMON_API __declspec(dllimport)
#endif

typedef struct tagAPPMONITOR_INFO
{
	CString		strFilePath;				// 文件路径	；请使用实际目录，无需考虑重定向问题
	CString		strProcessName;		// 进程名称
	BOOL		bAppActive;			//	是否有效
}APPMONITOR_INFO,*PAPPMONITOR_INFO;

typedef struct tagSESSION_PROCESS_INFO
{
	DWORD	dwSessionId;			// 会话ID
	CString		strFilePath;				//	文件路径
}SESSION_PROCESS_INFO,*PSESSION_PROCESS_INFO;

typedef CDuiList<APPMONITOR_INFO,APPMONITOR_INFO&> CAppMonitorInfo;
typedef CDuiList<SESSION_PROCESS_INFO,SESSION_PROCESS_INFO&> CSessionProcessInfo;

class /*DEAMON_API*/ CAppMonitor
{
public:
	static CAppMonitor* GetInstance();
protected:
	CAppMonitor();
	virtual ~CAppMonitor();

public:
	// 开始运行
	BOOL Run(LPWSTR lpszSrvName);

	// 设置服务状态
	BOOL SetSCManager(DWORD dwStatus, DWORD dwExitCode, DWORD dwProgress);

	/************************************************************************/
	/* 函数功能：添加需要保护的进程                                                               */
	/************************************************************************/
	BOOL AddMonitorAppInfo(APPMONITOR_INFO& AppMonitorInfo);
	//需要删除保护的进程信息
	BOOL DeleteMonitorAppInfo(const APPMONITOR_INFO& AppMonitorInfo);

	//设置服务接受的控制命令
	void SetAcceptCodes(DWORD dwAcceptCode);
	//获取服务接受的控制命令
	DWORD GetAcceptCodes();

	//设置轮训时间
	void SetPollTime(DWORD dwPollTime);
	//获取线程轮训时间
	DWORD GetPollTime();

	//停止监视
	BOOL StopMonitor();
	//开启监视
	BOOL StartMonitor();

	BOOL StartApps();
	BOOL IsAppRunning();

	//获取当前用户使用的SessionId
	DWORD GetActiveConsoleSessionId();
	BOOL QueryUserToken(DWORD dwSessionId, PHANDLE phToken);

	void OnSessionChange(DWORD dwEventType, LPVOID lpEventData);

	// 应用程序是否启用监视线程
	static DWORD ThreadMonitor(LPVOID lpParam);

	// 处理服务控制代码
	static DWORD ServiceHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID  lpContext);
protected:
	DWORD m_dwPollTime;								// 轮训时间
	DWORD m_dwCurrentState;						// 服务当前状态
	DWORD m_dwAcceptCodes;						// 接受处理的控制代码
	HANDLE m_hEventExit;								// Event事件句柄
	HANDLE m_hThreadMonitor;						// 线程句柄
	CRITICAL_SECTION m_csMonitor;				// 临界互斥
	static CAppMonitorInfo		m_AppMonitorList;
	CSessionProcessInfo	m_AppSessionList;
	SERVICE_STATUS_HANDLE m_hService;	// 服务句柄
};