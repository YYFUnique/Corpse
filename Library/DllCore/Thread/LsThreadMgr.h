#pragma once

#include "LsThread.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//template class __declspec(dllexport) CLsList<CLsThread*,CLsThread*>;

//对CLsThread类的管理
typedef CDuiList<CLsThread* , CLsThread*> CLsThreadList;

class DLL_API CLsThreadMgr
{
public:
	CLsThreadMgr(void);

	~CLsThreadMgr(void);

	//进行自身的初始化操作
	BOOL Initialize();

	//增加一个线程到链表
	BOOL AddThreadToList(CLsThread* pLsThread);

	//从链表删除一个线程(只是从链表中摘除，不负责停止线程及回收资源)
	BOOL RemoveThreadFromList(CLsThread* pLsThread);

	//从链表删除一个线程(从链表中摘除，并停止线程及回收资源)
	BOOL RemoveThreadFromList(THREAD_APP_ID LsThreadAppId , DWORD dwWaitTime = 2000);

	//删除所有线程(停止并回收所有线程对象资源)
	BOOL DeleteAllThread();

	//根据线程ID查找线程
	CLsThread* FindThreadById(THREAD_APP_ID LsThreadAppId , BOOL bRemoveFromList);

	//返回管理对象是否完成初始化
	BOOL IsInitComplete();

protected:
	//static BOOL CjThreadProc(LPVOID lParam);
protected:

	CLsThreadList m_LsThreadList;

	//临界区标识
	CRITICAL_SECTION m_CriticalSection;

	//是否完成初始化
	BOOL m_bInitComplete;
};

DLL_API CLsThreadMgr& GetLsThreadMgr();