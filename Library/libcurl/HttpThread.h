#pragma once

#include "HTTPRequest.h"

class HttpThreadPool;
class HttpThread
{
public:
	HttpThread(HttpThreadPool* pool);
	~HttpThread();

	void SetRequest(IHttpRequest* pRequest);
	BOOL Suspend();
	BOOL Resume();
	void Close();

protected:
	static UINT CALLBACK WorkThread(LPVOID lParam);
private:
	HANDLE m_Thread;
	HttpThreadPool* m_pPool;
	IHttpRequest* m_pRequest;
	bool m_bClose;
};

