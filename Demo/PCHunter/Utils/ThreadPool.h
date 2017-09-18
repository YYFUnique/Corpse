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
		CTask				TaskList;								//任务队列
		CResult			ResultList;							//任务结果列表
		ThreadPoolTaskFun pTaskFun;					//任务队列中传入函数的参数
	}ThreadParam,*PThreadParam;

	BOOL					m_bStartNow;
	UINT					m_uMsg;								//发送任务处理完毕消息
	UINT					m_uThreadNum;					//总线程数, 不包括管理线程
	ThreadParam*		m_pThreadParam;
	HANDLE*				m_phThread;
	HANDLE              m_hTaskEvent;
	BOOL					m_bStoped;
	CRITICAL_SECTION m_CriticalSection;
	LPVOID				m_lParam;							//回调函数参数
	LPTASKJOBRESULT_NOTIFY m_pTaskNotify;	//任务产生结果通知回调
	LPTASKJOBFINSH_NOTIFY    m_pFinshNotify;//任务结束回调
};
#include "ThreadPool.template"