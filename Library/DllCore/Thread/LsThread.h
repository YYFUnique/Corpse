#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//线程功能ID
typedef enum tagTHREAD_APP_ID
{
	THREAD_APP_ID_UNKNOWN,												//	未知线程
	THREAD_APP_ID_LOGIN,														//	主程序连接线程
	THREAD_APP_ID_HEART_BEAT,												//	心跳包线程
	THREAD_APP_ID_UPDATE_ICON_INFO,									//	下载客户端显示图标
	THREAD_APP_ID_REPORT_EVENT_INFO,								//	上报安全事件
	THREAD_APP_ID_REPORT_SAFETY_RULE_SUCCESS_RESULT,//	上报安全事件成功结果
	THREAD_APP_ID_LS_RULE_REPORT_INFO,								//	上报LsRule事件
	THREAD_APP_ID_LS_RULE_PRINT_MGR,								//	打印管理
	THREAD_APP_ID_ROUTE_CHANGED,										//	路由变化通知处理线程
	THREAD_APP_ID_ENCRYPT,													//	终端安全手机客户端压缩线程
}THREAD_APP_ID;

class DLL_API CLsThread
{
public:
	CLsThread(void);

	virtual ~CLsThread(void);

	//获取线程ID
	THREAD_APP_ID GetThreadId();

	//设置线程ID
	void SetThreadId(THREAD_APP_ID ThreadAppId);

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
	THREAD_APP_ID m_ThreadAppId;	

	//外部通知线程退出
	BOOL m_bExitThread;

	//外部通知同步事件
	HANDLE m_NotificationEvent;
};
