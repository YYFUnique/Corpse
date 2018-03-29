#include "StdAfx.h"
#include "LsThread.h"
#include "../Utils/ErrorInfo.h"

CLsThread::CLsThread(void)
{
	m_hThread = NULL;
	m_ThreadAppId = THREAD_APP_ID_UNKNOWN;
	m_pfnThreadProc = NULL;
	m_pThreadParam = NULL;
	m_bExitThread = FALSE;

	m_NotificationEvent = CreateEvent(NULL , FALSE , FALSE , NULL);
}

CLsThread::~CLsThread(void)
{
	if (m_hThread)
		CloseHandle(m_hThread);
	if (m_NotificationEvent)
		CloseHandle(m_NotificationEvent);
}

//获取线程ID
THREAD_APP_ID CLsThread::GetThreadId()
{
	return m_ThreadAppId;
}

//设置线程ID
void CLsThread::SetThreadId(THREAD_APP_ID LsThreadAppId)
{
	m_ThreadAppId = LsThreadAppId;
}

//设置线程参数信息
void CLsThread::SetThreadParam(LPVOID pThreadParam)
{
	m_pThreadParam = pThreadParam;
}

//启动线程
BOOL CLsThread::StartThread()
{
	BOOL bSuccess = FALSE;

	do 
	{
		if (m_hThread)
		{
			SetErrorInfo(CUSTOM_ERROR , 0 , _T("启动线程失败:上一个线程实例还未停止."));
			break;
		}

		m_bExitThread = FALSE;

		LPTHREAD_START_ROUTINE LsThreadProc = m_pfnThreadProc;
		LPVOID pThreadParam = m_pThreadParam;

		if (LsThreadProc == NULL)
		{
			LsThreadProc = (LPTHREAD_START_ROUTINE)DefaultThreadFun;
			pThreadParam = this;
		}

		m_hThread = CreateThread(NULL, 0, LsThreadProc, pThreadParam, CREATE_SUSPENDED, NULL);
		if (m_hThread == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("启动线程失败"));
			break;
		}

		ResumeThread(m_hThread);

		bSuccess = TRUE;
	} while (0);

	return bSuccess;
}

BOOL CLsThread::SetThreadStopFlag()
{
	m_bExitThread = TRUE;
	return TRUE;
}

//停止线程
BOOL CLsThread::StopThread(DWORD dwWaitTime , BOOL bTerminateThreadWhenWaitTimeout)
{
	m_bExitThread = TRUE;

	SetEvent(m_NotificationEvent);

	if (m_hThread == NULL)
		return TRUE;

	DWORD dwRet = WaitForSingleObject(m_hThread , dwWaitTime);
	if (dwRet == WAIT_TIMEOUT)
	{
		if (bTerminateThreadWhenWaitTimeout)
		{
			TerminateThread(m_hThread , 0);
			dwRet = WAIT_OBJECT_0;
		}
		else
		{
			SetErrorInfo(CUSTOM_ERROR , 0 , _T("终止线程失败:等待线程退出超时."));
			return FALSE;
		}
	}

	if (dwRet != WAIT_OBJECT_0)
	{
		SetErrorInfo(CUSTOM_ERROR , 0 , _T("终止线程时等待线程退出失败"));
		return FALSE;
	}

	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	m_pThreadParam = NULL;
	return TRUE;
}

BOOL CLsThread::SetNotificationEvent()
{
	if (SetEvent(m_NotificationEvent) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("设置线程通知事件失败"));
		return FALSE;
	}
	return TRUE;
}

UINT CLsThread::DefaultThreadFun(void* pPara)
{
	CLsThread* pPrinterMgr = (CLsThread*)pPara;
	DWORD dwRet = pPrinterMgr->ThreadProc();
	ReleaseThreadErrorInfo();
	return dwRet;
}

UINT CLsThread::ThreadProc()
{
	return 0;
}