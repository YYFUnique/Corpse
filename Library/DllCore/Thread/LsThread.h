#pragma once

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif

//线程功能ID
typedef enum _YJ_THREAD_APP_ID
{
	YJ_THREAD_APP_ID_UNKNOWN,													//未知线程
	YJ_THREAD_APP_ID_LOGIN,															//主程序连接线程
	YJ_THREAD_APP_ID_HEART_BEAT,													//心跳包线程
	YJ_THREAD_APP_ID_UPDATE_ICON_INFO,									//下载客户端显示图标
	YJ_THREAD_APP_ID_REPORT_EVENT_INFO,									//上报安全事件
	YJ_THREAD_APP_ID_REPORT_SAFETY_RULE_SUCCESS_RESULT,	//上报安全事件成功结果
	YJ_THREAD_APP_ID_LS_RULE_REPORT_INFO,								//上报LsRule事件
	YJ_THREAD_APP_ID_LS_RULE_PRINT_MGR,									//打印管理
	YJ_THREAD_APP_ID_ROUTE_CHANGED,										//路由变化通知处理线程
	LS_THREAD_APP_ID_ENCRYPT,														//终端安全手机客户端压缩线程
}YJ_THREAD_APP_ID;

class YCLIB_CORE_API CLsThread
{
public:
	CLsThread(void);

	virtual ~CLsThread(void);

	//获取线程ID
	YJ_THREAD_APP_ID GetThreadId();

	//设置线程ID
	void SetThreadId(YJ_THREAD_APP_ID LsThreadAppId);

	//设置线程参数信息
	void SetThreadParam(LPVOID pThreadParam);

	//启动线程
	virtual BOOL StartThread();

	//设置线程停止标记
	virtual BOOL SetThreadStopFlag();

	//停止线程
	virtual BOOL StopThread(DWORD dwWaitTime = INFINITE, BOOL bTerminateThreadWhenWaitTimeout = FALSE);

	//默认线程函数
	virtual UINT ThreadProc();

	//设置通知(同步)事件为有信号状态
	BOOL SetNotificationEvent();

protected:
	HANDLE m_hThread;

	//默认线程函数,自动调用ThreadProc()函数
	static UINT DefaultThreadFun(LPVOID);

	//工作线程入口函数(为空时调用DefaultThreadFun函数)
	LPTHREAD_START_ROUTINE m_pfnThreadProc;

	//线程参数内存指针(仅当m_pfnThreadProc不为空时有效)
	LPVOID m_pThreadParam;

	//线程ID标识(仅作为标识，不会对实际运行造成任何影响)
	YJ_THREAD_APP_ID m_LsThreadAppId;	

	//外部通知线程退出
	BOOL m_bExitThread;

	//外部通知同步事件
	HANDLE m_NotificationEvent;
};
