#pragma once

typedef enum tagSVR_TYPE
{
	SERVICE_TYPE_KERNEL_DRIVER						= 0x1,		//The service is a device driver.
	SERVICE_TYPE_FILE_SYSTEM_DRIVER				= 0x2,		//The service is a file system driver.
	SERVICE_TYPE_WIN32_OWN_PROCESS			= 0x10,	//The service runs in its own process.
	SERVICE_TYPE_WIN32_SHARE_PROCESS		= 0x20,	//The service shares a process with other services.
}SVR_TYPE;

typedef enum tagSVR_STARTTYPE
{
	BOOT_START = 0,			//引导启动
	SYSTEM_START = 1,		//系统启动
	AUTO_START = 2,			//自动启动
	DEMAND_START = 3,		//手动启动
	DISABLED_START = 4,	//禁止启动
}SVR_STARTTYPE;

typedef enum tagSVR_STATE
{
	STATE_STOPPED = 1,			//The service has stopped.
	START_PENDING = 2,			//The service is starting.
	STOP_PENDING = 3,			//The service is stopping.
	RUNNING = 4,					//The service is running.
	CONTINUE_PENDING = 5,	//The service is about to continue.
	PAUSE_PENDING = 6,			//The service is pausing.
	PAUSED = 7,						//The service is paused.
}SVR_STATE;

typedef CDuiList<CDuiString,LPCTSTR> CDependList;

typedef struct tagSERVICEINFO
{
	CDuiString strSvrName;				//服务名称
	CDuiString strDisplayName;		//显示名称
	CDuiString strDescription;			//服务描述
	CDuiString strPath;						//服务路径
	DWORD    dwPID;						//进程ID
	SVR_STATE    dwRunStatus;		//服务状态
	SVR_STARTTYPE    dwStartType;	//启动状态
	SVR_TYPE    dwType;					//服务类型
	//CStdValArray strDependencies; //依赖服务
	//CDependList	DependList;			//依赖服务
}SERVICEINFO;

typedef CDuiList<SERVICEINFO,const SERVICEINFO&> CSvrInfo;

class CServices : public CBase
{
public:
	CServices();
	~CServices();

public:
	virtual void Notify(TNotifyUI& msg);
	virtual void OnPaint();
	void OnMenu(TNotifyUI& msg);
protected:
	
	BOOL GetSvrInfo(CSvrInfo& SvrInfoList);

	void FormatPid(DWORD Pid, CDuiString& strPid);
	void FormatRunStatus(DWORD dwCurrentState, CDuiString& strRunState);
	void FormatStartType(DWORD dwStartType, CDuiString& strStartType);
protected:
	CSvrInfo	m_ServicesInfo;
};