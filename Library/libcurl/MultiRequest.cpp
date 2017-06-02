#include "MultiRequest.h"

#include "libcurl/curl/multi.h"

typedef struct tagMultiRequestParam
{
	CMultiRequest* pMulti;
	DWORD			   dwEvent;
}MultiRequestParam;

CMultiRequest::CMultiRequest()
{
	m_hSema = CreateSemaphore(NULL, 0, 1, NULL);
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)workThread, this, 0, NULL);
	m_dwTimeout = 3;
	m_pCURLM = NULL;//curl_multi_init();
	m_pLoop = NULL;
	//m_Timeout = NULL;
	m_hEvent = NULL;
	m_pCallback = NULL;	
	m_bExist = FALSE;
}

CMultiRequest::~CMultiRequest()
{
	Stop();
}

void CMultiRequest::Stop()
{
	m_bExist = TRUE;

	//非线程安全，以后寻找方案解决
	if (m_pLoop)
		uv_stop(m_pLoop);

	if (m_hEvent != NULL)
	{
		SetEvent(m_hEvent);
		m_hEvent = NULL;
	}

	if (m_hSema != NULL)
	{
		CloseHandle(m_hSema);
		m_hSema = NULL;
	}
}

void CMultiRequest::Init()
{
	do 
	{
		if (m_pCURLM != NULL)
			return;

		m_pCURLM = curl_multi_init();
		if (m_pCURLM == NULL)
			break;

		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		curl_multi_setopt(m_pCURLM, CURLMOPT_SOCKETFUNCTION, SocketFunc);
		curl_multi_setopt(m_pCURLM, CURLMOPT_SOCKETDATA, this);
		curl_multi_setopt(m_pCURLM, CURLMOPT_TIMERFUNCTION, TimeFunc);
		curl_multi_setopt(m_pCURLM, CURLMOPT_TIMERDATA, this);
	} while (FALSE);
}
 
BOOL CMultiRequest::doHttpGet(DWORD dwEvent, LPCTSTR lpszURL)
{
	BOOL bSuccess = FALSE;
	do 
	{ 
		CURL* pEasy = curl_easy_init();
		if (pEasy == NULL)
			break;

		::curl_easy_setopt(pEasy, CURLOPT_URL, CStringA(lpszURL));
		::curl_easy_setopt(pEasy, CURLOPT_HTTPGET, 1 );
		//::curl_easy_setopt(m_pCurl, CURLOPT_HEADER, dwHeader);
		if (m_dwTimeout != 0)
			::curl_easy_setopt(pEasy, CURLOPT_TIMEOUT_MS, m_dwTimeout*1000);
		::curl_easy_setopt(pEasy, CURLOPT_WRITEFUNCTION, &CMultiRequest::ProcessFunc); 

		//由于内存分配和释放不在一块，故HTTP失败的时候，
		//内存可能不能释放，暂时没有找到好的解决方案
		//目前通过测试发现

		MultiRequestParam* pMultiRequestParam = new MultiRequestParam;
		pMultiRequestParam->dwEvent = dwEvent;
		pMultiRequestParam->pMulti = this;

		::curl_easy_setopt(pEasy, CURLOPT_WRITEDATA, pMultiRequestParam);
		::curl_easy_setopt(pEasy, CURLOPT_PRIVATE, pMultiRequestParam);

		//临时解决和uv_run线程冲突问题
		WaitForSingleObject(m_hSema, INFINITE);

		if (m_pCURLM)
			curl_multi_add_handle(m_pCURLM, pEasy);

		ReleaseSemaphore(m_hSema, 1, NULL);

		SetEvent(m_hEvent);

		bSuccess = TRUE;
	} while (FALSE);
	
	return bSuccess;
}

void CMultiRequest::SetCallback(ILibcurlCallback* pCallback)
{
	m_pCallback = pCallback;
}

size_t CMultiRequest::ProcessFunc(LPVOID lpData, size_t size, size_t nitems, LPVOID lParam)
{
	size_t sizeRet = size * nitems;
	MultiRequestParam* pMultiRequestParam = (MultiRequestParam*)lParam;
	if (pMultiRequestParam->pMulti->m_pCallback)
		sizeRet = pMultiRequestParam->pMulti->m_pCallback->ProcessFunc(pMultiRequestParam->dwEvent, lpData, size, nitems);
	return sizeRet;
}

UINT CMultiRequest::SocketFunc(CURL* pEasy, curl_socket_t CurlSocket, int nAction, LPVOID lParam, LPVOID socketp)
{
	CMultiRequest* pMulti = (CMultiRequest*)lParam;

	CurlContext* pUvData;
	int nSocketEvents = 0;

	switch(nAction) 
	{
		case CURL_POLL_IN:
		case CURL_POLL_OUT:
		case CURL_POLL_INOUT:
				pUvData = socketp ? (CurlContext *) socketp : pMulti->CrateCurlContext(CurlSocket);

				curl_multi_assign(pMulti->m_pCURLM, CurlSocket, (void *) pUvData);

				if (nAction != CURL_POLL_IN)
					nSocketEvents |= UV_WRITABLE;
				if (nAction != CURL_POLL_OUT)
					nSocketEvents |= UV_READABLE;

				uv_poll_start(&pUvData->hPoll, nSocketEvents, doSocketAction);
			break;
		case CURL_POLL_REMOVE:
				if (socketp) 
				{
					pUvData = (CurlContext*)socketp;
					uv_poll_stop(&pUvData->hPoll);
					uv_close((uv_handle_t*)&pUvData->hPoll, _uvClose);
					curl_multi_assign(pMulti->m_pCURLM, CurlSocket, NULL);
				}
			break;
		default:
			abort();
	}

	return 0;
}

UINT CMultiRequest::TimeFunc(CURLM* pCURLM, LONG nTime, LPVOID lParam)
{
	CMultiRequest* pMulti = (CMultiRequest*)lParam;
	if (nTime < 0) 
	{
		uv_timer_stop(&pMulti->m_Timeout);
	}
	else 
	{
		if (nTime == 0)
			nTime = 1; /* 0 means directly call socket_action, but we'll do it
							 in a bit */
		uv_timer_start(&pMulti->m_Timeout, OnTimeout, nTime, 0);
	}
	return 0;
}

void CMultiRequest::CheckMultiInfo(CURLM* pCURLM)
{
	CURLMsg* pMsg;
	int nPending;
	CURL* pEasy;

	while ((pMsg = curl_multi_info_read(pCURLM, &nPending)))
	{
		switch (pMsg->msg) 
		{
			case CURLMSG_DONE:
				/* Do not use message data after calling curl_multi_remove_handle() and
				curl_easy_cleanup(). As per curl_multi_info_read() docs:
				"WARNING: The data the returned pointer points to will not survive
				calling curl_multi_cleanup, curl_multi_remove_handle or
				curl_easy_cleanup." */
				pEasy = pMsg->easy_handle;

				MultiRequestParam* pMultiRequestPrivateParam;
				curl_easy_getinfo(pEasy, CURLINFO_PRIVATE, &pMultiRequestPrivateParam);

				//释放内存
				delete pMultiRequestPrivateParam;
				
				curl_multi_remove_handle(pCURLM, pEasy);
				curl_easy_cleanup(pEasy);
			break;
		}
	}
}

void CMultiRequest::OnTimeout(uv_timer_t *hTimer)
{
	//
	CMultiRequest* pMulti = (CMultiRequest*)hTimer->data;
	int nRun = 0;
	if (pMulti->m_pCURLM)
		curl_multi_socket_action(pMulti->m_pCURLM, CURL_SOCKET_TIMEOUT, 0,&nRun);
	CheckMultiInfo(pMulti->m_pCURLM);
}

CurlContext* CMultiRequest::CrateCurlContext(curl_socket_t CurlSockFd)
{
	CurlContext* pUvData;

	pUvData = (CurlContext *)malloc(sizeof(CurlContext));
	if (pUvData == NULL)
		return pUvData;
	
	pUvData->CurlSocketFd = CurlSockFd;
	pUvData->pCURLM = m_pCURLM;
	uv_poll_init_socket(m_pLoop, &pUvData->hPoll, CurlSockFd);
	pUvData->hPoll.data = pUvData;

	return pUvData;
}

void CMultiRequest::doSocketAction(uv_poll_t* uvPollData, int status, int nEvents)
{
	int nRun;
	int nSocketFlags = 0;
	CurlContext* pUvData;

	if (nEvents & UV_READABLE)
		nSocketFlags |= CURL_CSELECT_IN;
	if (nEvents & UV_WRITABLE)
		nSocketFlags |= CURL_CSELECT_OUT;

	pUvData = (CurlContext *)uvPollData->data;

	curl_multi_socket_action(pUvData->pCURLM, pUvData->CurlSocketFd, nSocketFlags, &nRun);

	CheckMultiInfo(pUvData->pCURLM);
}

void CMultiRequest::_uvClose(uv_handle_t* handle)
{
	CurlContext* pUvData = (CurlContext*)handle->data;
	if (pUvData)
		free(pUvData);
}

UINT CMultiRequest::workThread(LPVOID lParam)
{
	CMultiRequest* pMulti = (CMultiRequest*)lParam;
	/*WaitForSingleObject(pMulti->m_hSema, INFINITE);*/

	pMulti->m_pLoop = uv_loop_new();
	uv_timer_init(pMulti->m_pLoop, &pMulti->m_Timeout);
	pMulti->m_Timeout.data = pMulti;

	ReleaseSemaphore(pMulti->m_hSema, 1, NULL);
	do 
	{
		WaitForSingleObject(pMulti->m_hEvent,INFINITE);
		if (pMulti->m_bExist)
			break;
		uv_run(pMulti->m_pLoop, UV_RUN_DEFAULT);
	} while (TRUE);
	//uv_loop_close(pMulti->m_pLoop);
	pMulti->m_pLoop = NULL;
	return 0;
}