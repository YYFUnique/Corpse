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
	BOOT_START = 0,			//��������
	SYSTEM_START = 1,		//ϵͳ����
	AUTO_START = 2,			//�Զ�����
	DEMAND_START = 3,		//�ֶ�����
	DISABLED_START = 4,	//��ֹ����
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
	CDuiString strSvrName;				//��������
	CDuiString strDisplayName;		//��ʾ����
	CDuiString strDescription;			//��������
	CDuiString strPath;						//����·��
	DWORD    dwPID;						//����ID
	SVR_STATE    dwRunStatus;		//����״̬
	SVR_STARTTYPE    dwStartType;	//����״̬
	SVR_TYPE    dwType;					//��������
	//CStdValArray strDependencies; //��������
	//CDependList	DependList;			//��������
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