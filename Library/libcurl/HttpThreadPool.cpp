#include "stdafx.h"
#include "HttpThreadPool.h"
#include "HttpThread.h"

HttpThreadPool::HttpThreadPool()
{
}


HttpThreadPool::~HttpThreadPool()
{

}

HttpThread* HttpThreadPool::Pop()
{
	HttpThread* p = NULL;
	if (m_FreeThread.empty())
	{
		p = AllocThread();
		return p;
	}

	p = m_FreeThread.top();
	return p;
}

void HttpThreadPool::Push(HttpThread* pThread)
{
	pThread->Suspend();
	m_FreeThread.push(pThread);
}

HttpThread* HttpThreadPool::AllocThread()
{
	HttpThread* pThread = new HttpThread(this);

	m_Thread.push_back(pThread);

	return pThread;
}
