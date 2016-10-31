#pragma once

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif

//�̹߳���ID
typedef enum _YJ_THREAD_APP_ID
{
	YJ_THREAD_APP_ID_UNKNOWN,													//δ֪�߳�
	YJ_THREAD_APP_ID_LOGIN,															//�����������߳�
	YJ_THREAD_APP_ID_HEART_BEAT,													//�������߳�
	YJ_THREAD_APP_ID_UPDATE_ICON_INFO,									//���ؿͻ�����ʾͼ��
	YJ_THREAD_APP_ID_REPORT_EVENT_INFO,									//�ϱ���ȫ�¼�
	YJ_THREAD_APP_ID_REPORT_SAFETY_RULE_SUCCESS_RESULT,	//�ϱ���ȫ�¼��ɹ����
	YJ_THREAD_APP_ID_LS_RULE_REPORT_INFO,								//�ϱ�LsRule�¼�
	YJ_THREAD_APP_ID_LS_RULE_PRINT_MGR,									//��ӡ����
	YJ_THREAD_APP_ID_ROUTE_CHANGED,										//·�ɱ仯֪ͨ�����߳�
	LS_THREAD_APP_ID_ENCRYPT,														//�ն˰�ȫ�ֻ��ͻ���ѹ���߳�
}YJ_THREAD_APP_ID;

class YCLIB_CORE_API CLsThread
{
public:
	CLsThread(void);

	virtual ~CLsThread(void);

	//��ȡ�߳�ID
	YJ_THREAD_APP_ID GetThreadId();

	//�����߳�ID
	void SetThreadId(YJ_THREAD_APP_ID LsThreadAppId);

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
	YJ_THREAD_APP_ID m_LsThreadAppId;	

	//�ⲿ֪ͨ�߳��˳�
	BOOL m_bExitThread;

	//�ⲿ֪ͨͬ���¼�
	HANDLE m_NotificationEvent;
};
