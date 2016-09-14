#include "StdAfx.h"
#include "LsThreadMgr.h"
#include "../Utils/ErrorInfo.h"

CLsThreadMgr& GetLsThreadMgr()
{
	static CLsThreadMgr s_LsThreadMgr;

	if (s_LsThreadMgr.IsInitComplete() == FALSE)
		s_LsThreadMgr.Initialize();

	return s_LsThreadMgr;
}

CLsThreadMgr::CLsThreadMgr(void)
{
	m_bInitComplete = FALSE;//构造函数不进行实际的初始化，避免全局变量初始化顺序带来的问题
}

CLsThreadMgr::~CLsThreadMgr(void)
{
	if (m_bInitComplete)
		DeleteCriticalSection(&m_CriticalSection);
}

BOOL CLsThreadMgr::IsInitComplete()
{
	return m_bInitComplete;
}

BOOL CLsThreadMgr::Initialize()
{
	//判断是否已完成过初始化
	if (m_bInitComplete)
		return TRUE;

	if (InitializeCriticalSectionAndSpinCount(&m_CriticalSection , 0x80000400) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("初始化线程管理对象临界区失败"));
		return FALSE;
	}

	m_bInitComplete = TRUE;
	return TRUE;
}

BOOL CLsThreadMgr::AddThreadToList(CLsThread* pLsThread)
{
	EnterCriticalSection(&m_CriticalSection);
	m_LsThreadList.AddTail(pLsThread);
	LeaveCriticalSection(&m_CriticalSection);

	return TRUE;
}

BOOL CLsThreadMgr::RemoveThreadFromList(CLsThread* pLsThread)
{
	EnterCriticalSection(&m_CriticalSection);
	POSITION pos = m_LsThreadList.GetHeadPosition();
	while(pos)
	{
		if (m_LsThreadList.GetAt(pos) == pLsThread)
		{
			m_LsThreadList.RemoveAt(pos);
			break;
		}

		m_LsThreadList.GetNext(pos);
	}
	LeaveCriticalSection(&m_CriticalSection);

	return pos != NULL;//返回是否进行删除操作
}

BOOL CLsThreadMgr::RemoveThreadFromList(YJ_THREAD_APP_ID LsThreadAppId , DWORD dwWaitTime)
{
	BOOL bRemoveThread = FALSE;

	EnterCriticalSection(&m_CriticalSection);
	POSITION pos = m_LsThreadList.GetHeadPosition();
	while(pos)
	{
		POSITION curpos = pos;
		CLsThread* pLsThread = m_LsThreadList.GetNext(pos);
		if (pLsThread->GetThreadId() != LsThreadAppId)
			continue;

		pLsThread->SetThreadStopFlag();
		pLsThread->StopThread(dwWaitTime , TRUE);
		delete pLsThread;
		bRemoveThread = TRUE;

		m_LsThreadList.RemoveAt(curpos);
	}
	LeaveCriticalSection(&m_CriticalSection);

	return bRemoveThread;//返回是否进行删除操作
}

BOOL CLsThreadMgr::DeleteAllThread()
{
	BOOL bSuccess = TRUE;

	EnterCriticalSection(&m_CriticalSection);
	POSITION pos = m_LsThreadList.GetHeadPosition();
	while(pos)
	{
		CLsThread* pLsThread = m_LsThreadList.GetNext(pos);
		if (pLsThread->SetThreadStopFlag() == FALSE)
			bSuccess = FALSE;
	}

	pos = m_LsThreadList.GetHeadPosition();
	while(pos)
	{
		CLsThread* pLsThread = m_LsThreadList.GetNext(pos);
		if (pLsThread->StopThread() == FALSE)
			bSuccess = FALSE;
		delete pLsThread;
	}

	m_LsThreadList.RemoveAll();

	LeaveCriticalSection(&m_CriticalSection);
	return bSuccess;
}

CLsThread* CLsThreadMgr::FindThreadById(YJ_THREAD_APP_ID LsThreadAppId , BOOL bRemoveFromList)
{
	CLsThread* pReturnThread = NULL;

	EnterCriticalSection(&m_CriticalSection);
	POSITION pos = m_LsThreadList.GetHeadPosition();
	while(pos)
	{
		CLsThread* pLsThread = m_LsThreadList.GetAt(pos);
		if (pLsThread->GetThreadId() == LsThreadAppId)
		{
			pReturnThread = pLsThread;
			if (bRemoveFromList)
				m_LsThreadList.RemoveAt(pos);
			break;
		}

		m_LsThreadList.GetNext(pos);
	}
	LeaveCriticalSection(&m_CriticalSection);
	return pReturnThread;
}

/*BOOL CLsThreadMgr::CjThreadProc(LPVOID lParam)
{
	CLsThreadMgr* pThreadMgr = (CLsThreadMgr*)lParam;
	if (pThreadMgr == NULL)
		return FALSE;

	while(TRUE)
	{
		EnterCriticalSection(&m_CriticalSection);
		POSITION pos = m_LsThreadList.GetHeadPosition();
		while(pos)
		{
			DWORD dwExist = 0;
			const CLsThread* pThread = m_LsThreadList.GetNext(pos);
			WaitForSingleObject(pThread->m_hThread,0)
			if (GetExitCodeThread(pThread->m_hThread,&dwExist) != FALSE)
				//if (dwExist != STILL_ACTIVE)
				GetLastError();
					//pThread->StopThread()
		}
		LeaveCriticalSection(&m_CriticalSection);
		Sleep(500);
	}

	return TRUE;
}*/