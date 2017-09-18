#pragma once
#include "DllCore/Utils/DuiList.h"
#include <vector>
using namespace std;

typedef void (*LPTASKJOBRESULT_NOTIFY)(LPVOID lParam);
typedef void (*LPTASKJOBFINSH_NOTIFY)(LPVOID lParam);

typedef LPTHREAD_START_ROUTINE  ThreadPoolTaskFun;

template<class CTaskType, class CResultType>
class CThreadPoolMgr
{
public:
	CThreadPoolMgr(UINT nThreadNum, ThreadPoolTaskFun pTaskFun,BOOL bStartNow =TRUE);
	~CThreadPoolMgr();
public:
	BOOL AddJob(CTaskType& TaskInfo);
	BOOL GetJobInfo(CTaskType& TaskInfo);

	BOOL AddResult(CResultType& ResultInfo);
	BOOL GetResultInfo(CResultType& ResultInfo);

	BOOL TerminateAll();
	BOOL IsStoped();

	BOOL RunThread();
	HANDLE GetJobEvent();
	UINT GetJobCount();
	BOOL RegisterNotify(LPTASKJOBRESULT_NOTIFY lpTaskJobNotify, 
									LPTASKJOBFINSH_NOTIFY lpTaskFinshNotify, LPVOID lpParameter);
protected:
	BOOL m_bFinished;
protected:
	typedef CDuiList<CTaskType,CTaskType&> CTask;
	typedef CDuiList<CResultType,CResultType&> CResult;
	typedef struct ThreadParam
	{
		CTask				TaskList;								//�������
		CResult			ResultList;							//�������б�
		ThreadPoolTaskFun pTaskFun;					//��������д��뺯���Ĳ���
	}ThreadParam,*PThreadParam;

	BOOL					m_bStartNow;
	UINT					m_uMsg;								//���������������Ϣ
	UINT					m_uThreadNum;					//���߳���, �����������߳�
	ThreadParam*		m_pThreadParam;
	HANDLE*				m_phThread;
	HANDLE              m_hTaskEvent;
	BOOL					m_bStoped;
	CRITICAL_SECTION m_CriticalSection;
	LPVOID				m_lParam;							//�ص���������
	LPTASKJOBRESULT_NOTIFY m_pTaskNotify;	//����������֪ͨ�ص�
	LPTASKJOBFINSH_NOTIFY    m_pFinshNotify;//��������ص�
};
#include "ThreadPool.template"