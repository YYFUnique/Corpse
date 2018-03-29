#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//�̹߳���ID
typedef enum tagTHREAD_APP_ID
{
	THREAD_APP_ID_UNKNOWN,												//	δ֪�߳�
	THREAD_APP_ID_LOGIN,														//	�����������߳�
	THREAD_APP_ID_HEART_BEAT,												//	�������߳�
	THREAD_APP_ID_UPDATE_ICON_INFO,									//	���ؿͻ�����ʾͼ��
	THREAD_APP_ID_REPORT_EVENT_INFO,								//	�ϱ���ȫ�¼�
	THREAD_APP_ID_REPORT_SAFETY_RULE_SUCCESS_RESULT,//	�ϱ���ȫ�¼��ɹ����
	THREAD_APP_ID_LS_RULE_REPORT_INFO,								//	�ϱ�LsRule�¼�
	THREAD_APP_ID_LS_RULE_PRINT_MGR,								//	��ӡ����
	THREAD_APP_ID_ROUTE_CHANGED,										//	·�ɱ仯֪ͨ�����߳�
	THREAD_APP_ID_ENCRYPT,													//	�ն˰�ȫ�ֻ��ͻ���ѹ���߳�
}THREAD_APP_ID;

class DLL_API CLsThread
{
public:
	CLsThread(void);

	virtual ~CLsThread(void);

	//��ȡ�߳�ID
	THREAD_APP_ID GetThreadId();

	//�����߳�ID
	void SetThreadId(THREAD_APP_ID ThreadAppId);

	//�����̲߳�����Ϣ
	void SetThreadParam(LPVOID pThreadParam);

	//�����߳�
	virtual BOOL StartThread();

	//�����߳�ֹͣ���
	virtual BOOL SetThreadStopFlag();

	//ֹͣ�߳�
	virtual BOOL StopThread(DWORD dwWaitTime = INFINITE, BOOL bTerminateThreadWhenWaitTimeout = FALSE);

	//Ĭ���̺߳���
	virtual UINT ThreadProc();

	//����֪ͨ(ͬ��)�¼�Ϊ���ź�״̬
	BOOL SetNotificationEvent();

protected:
	HANDLE m_hThread;

	//Ĭ���̺߳���,�Զ�����ThreadProc()����
	static UINT DefaultThreadFun(LPVOID);

	//�����߳���ں���(Ϊ��ʱ����DefaultThreadFun����)
	LPTHREAD_START_ROUTINE m_pfnThreadProc;

	//�̲߳����ڴ�ָ��(����m_pfnThreadProc��Ϊ��ʱ��Ч)
	LPVOID m_pThreadParam;

	//�߳�ID��ʶ(����Ϊ��ʶ�������ʵ����������κ�Ӱ��)
	THREAD_APP_ID m_ThreadAppId;	

	//�ⲿ֪ͨ�߳��˳�
	BOOL m_bExitThread;

	//�ⲿ֪ͨͬ���¼�
	HANDLE m_NotificationEvent;
};
