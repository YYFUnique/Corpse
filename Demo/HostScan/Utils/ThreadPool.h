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
		CTask				TaskList;								//任务队列
		CResult			ResultList;							//任务结果列表
		ThreadPoolTaskFun pTaskFun;					//任务队列中传入函数的参数
	}ThreadParam,*PThreadParam;

	BOOL					m_bStartNow;
	HWND					m_hMainWnd;
	UINT					m_uMsg;								//发送任务处理完毕消息
	UINT					m_uThreadNum;					//总线程数, 不包括管理线程
	ThreadParam*		m_pThreadParam;
	HANDLE*				m_phThread;
	HANDLE              m_hAddTaskEvent;
	BOOL					m_bStoped;
	CRITICAL_SECTION m_CriticalSection;
};
#include "ThreadPool.template"