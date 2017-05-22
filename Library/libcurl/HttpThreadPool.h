#pragma once
#include <vector>
#include <stack>
class HttpThread;
class HttpThreadPool
{
public:
	HttpThreadPool();
	~HttpThreadPool();

	HttpThread* Pop();
	void Push(HttpThread* thread);

private:
	HttpThread* AllocThread();

private:
	std::vector<HANDLE > m_Thread;
	std::stack<HttpThread*> m_FreeThread;
};

