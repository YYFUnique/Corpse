#pragma once
#include "LsList.h"
#include <vector>
using namespace std;

typedef LPTHREAD_START_ROUTINE  ThreadPoolTaskFun;

template<class CTypeTask, class CTypeResult>
class CThreadPoolMgr
{
public:
	CThreadPoolMgr(UINT nThreadNum, ThreadPoolTaskFun pTaskFun,BOOL bStartNow =TRUE);
	~CThreadPoolMgr();
public:
	BOOL AddTaskToQueue(CTypeTask& TaskInfo);
	BOOL GetTaskJobInfo(CTypeTask& TaskInfo);

	BOOL AddTaskResultToQueue(CTypeResult& ResultInfo);
	BOOL GetTaskResultInfo(CTypeResult& ResultInfo);

	BOOL StopAllTaskQueue();
	BOOL IsTaskStoped();

	HANDLE GetAddTaskJobEvent();
	UINT GetTaskJobCount();
	BOOL RegisterNotify(HWND hMainWnd,UINT uMsg);
protected:
	
protected:
	typedef CLsList<CTypeTask,CTypeTask&> CTask;
	typedef CLsList<CTypeResult,CTypeResult&> CResult;
	typedef struct ThreadParam
	{
		CTask				TaskList;								//�������
		CResult			ResultList;							//�������б�
		ThreadPoolTaskFun pTaskFun;					//��������д��뺯���Ĳ���
	}ThreadParam,*PThreadParam;

	BOOL					m_bStartNow;
	HWND					m_hMainWnd;
	UINT					m_uMsg;								//���������������Ϣ
	UINT					m_uThreadNum;					//���߳���, �����������߳�
	ThreadParam*		m_pThreadParam;
	HANDLE*				m_phThread;
	HANDLE              m_hAddTaskEvent;
	BOOL					m_bStoped;
	CRITICAL_SECTION m_CriticalSection;
};
#include "ThreadPool.template"