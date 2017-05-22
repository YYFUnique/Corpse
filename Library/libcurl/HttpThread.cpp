#include "stdafx.h"
#include "HttpThread.h"
#include "HttpThreadPool.h"
#include "HttpManager.h"
#include "HttpRequest.h"

HttpThread::HttpThread(HttpThreadPool* pool)
:m_pPool(pool)
{
	DWORD  dwThreadID; 
	m_Thread = (HANDLE)CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WorkThread,this,NULL,&dwThreadID);
	Suspend();
}

HttpThread::~HttpThread()
{
	WaitForSingleObject(m_Thread, INFINITE);
}

UINT HttpThread::WorkThread(LPVOID lParam)
{
	HttpThread* pThreadObj = (HttpThread*)lParam;

	//while (!pThreadObj->m_bClose)
	{
		if (pThreadObj->m_pRequest)
		{
			pThreadObj->m_pRequest->Run();
			pThreadObj->m_pRequest = NULL;
		}

		//¹ÒÆð
		pThreadObj->m_pPool->Push(pThreadObj);
	}
	return 0;
}

void HttpThread::SetRequest(IHttpRequest* pRequest)
{
	m_pRequest = pRequest;
}

BOOL HttpThread::Suspend()
{
	return SuspendThread(m_Thread);
}

BOOL HttpThread::Resume()
{
	ResumeThread(m_Thread);
	return FALSE;
}

void HttpThread::Close()
{
	m_bClose = false;
	Resume();
}
