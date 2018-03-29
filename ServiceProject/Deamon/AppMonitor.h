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
	CString		strFilePath;				// �ļ�·��	����ʹ��ʵ��Ŀ¼�����迼���ض�������
	CString		strProcessName;		// ��������
	BOOL		bAppActive;			//	�Ƿ���Ч
}APPMONITOR_INFO,*PAPPMONITOR_INFO;

typedef struct tagSESSION_PROCESS_INFO
{
	DWORD	dwSessionId;			// �ỰID
	CString		strFilePath;				//	�ļ�·��
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
	// ��ʼ����
	BOOL Run(LPWSTR lpszSrvName);

	// ���÷���״̬
	BOOL SetSCManager(DWORD dwStatus, DWORD dwExitCode, DWORD dwProgress);

	/************************************************************************/
	/* �������ܣ������Ҫ�����Ľ���                                                               */
	/************************************************************************/
	BOOL AddMonitorAppInfo(APPMONITOR_INFO& AppMonitorInfo);
	//��Ҫɾ�������Ľ�����Ϣ
	BOOL DeleteMonitorAppInfo(const APPMONITOR_INFO& AppMonitorInfo);

	//���÷�����ܵĿ�������
	void SetAcceptCodes(DWORD dwAcceptCode);
	//��ȡ������ܵĿ�������
	DWORD GetAcceptCodes();

	//������ѵʱ��
	void SetPollTime(DWORD dwPollTime);
	//��ȡ�߳���ѵʱ��
	DWORD GetPollTime();

	//ֹͣ����
	BOOL StopMonitor();
	//��������
	BOOL StartMonitor();

	BOOL StartApps();
	BOOL IsAppRunning();

	//��ȡ��ǰ�û�ʹ�õ�SessionId
	DWORD GetActiveConsoleSessionId();
	BOOL QueryUserToken(DWORD dwSessionId, PHANDLE phToken);

	void OnSessionChange(DWORD dwEventType, LPVOID lpEventData);

	// Ӧ�ó����Ƿ����ü����߳�
	static DWORD ThreadMonitor(LPVOID lpParam);

	// ���������ƴ���
	static DWORD ServiceHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID  lpContext);
protected:
	DWORD m_dwPollTime;								// ��ѵʱ��
	DWORD m_dwCurrentState;						// ����ǰ״̬
	DWORD m_dwAcceptCodes;						// ���ܴ���Ŀ��ƴ���
	HANDLE m_hEventExit;								// Event�¼����
	HANDLE m_hThreadMonitor;						// �߳̾��
	CRITICAL_SECTION m_csMonitor;				// �ٽ绥��
	static CAppMonitorInfo		m_AppMonitorList;
	CSessionProcessInfo	m_AppSessionList;
	SERVICE_STATUS_HANDLE m_hService;	// ������
};