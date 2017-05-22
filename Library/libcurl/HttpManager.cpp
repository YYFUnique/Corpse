#include "stdafx.h"
#include "HttpManager.h"
#include "HttpThreadPool.h"
#include "HttpThread.h"

#include "libcurl/curl/curl.h"

CHTTPManager::CHTTPManager()
{
	m_pThreadPool = NULL;
	m_pHttpRequest = NULL;
	m_pLibcurlCallback = NULL;
}

CHTTPManager::~CHTTPManager()
{
	if (m_pThreadPool)
	{
		delete m_pThreadPool;
		m_pThreadPool = NULL;
	}
	curl_global_cleanup();
}

BOOL CHTTPManager::Init()
{
	BOOL bSuccess = FALSE;
	CURLcode LibcurlCode = CURLE_OK;
	do 
	{
		LibcurlCode = curl_global_init(CURL_GLOBAL_ALL);
		if (LibcurlCode != CURLE_OK)
			break;

		if (m_pThreadPool == NULL)
			m_pThreadPool = new HttpThreadPool();

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void CHTTPManager::SetLibcurlCallback(ILibcurlResponse* pCallback)
{
	m_pLibcurlCallback = pCallback;
}

BOOL CHTTPManager::doHttpGet(DWORD dwEvent, LPCTSTR lpszURL, LPCTSTR lpszData, DWORD dwTimeout /*=3*/)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_pThreadPool == NULL)
			break;

		HttpThread* pHTTPThread = m_pThreadPool->Pop();
		if (pHTTPThread == NULL)
			break;

		m_pHttpRequest = new CHTTPRequest;

		if (m_pHttpRequest)
		{
			if (m_pLibcurlCallback)
				m_pHttpRequest->SetCallback(m_pLibcurlCallback);
			m_pHttpRequest->SetTimeout(dwTimeout);
			m_pHttpRequest->Initialize(dwEvent, lpszURL, lpszData);
		}
		pHTTPThread->SetRequest(m_pHttpRequest);
		pHTTPThread->Resume();

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CHTTPManager::doHttpPost(DWORD dwEvent, LPCTSTR lpszURL, LPCTSTR lpszData, DWORD dwTimeout /*=3*/)
{
	return TRUE;
}

/*void CHTTPManager::SendRequest(IHttpRequest* pRequestObj)
{
	if (pRequestObj == NULL)
		return;

	if (m_pThreadPool == NULL)
		Init();

	HttpThread* pHTTPThread = m_pThreadPool->Pop();
	if (pHTTPThread == NULL)
		return;

	pHTTPThread->SetRequest(pRequestObj);
	pHTTPThread->Resume();
}*/

// 
// HttpBaseRequest::HttpBaseRequest()
// {
// 	curl = curl_easy_init();
// 	m_nOutTime = 0;
// 	m_nRetry = 0;
// 	m_eMsgType = E_Requset_Type::e_Req_Post;
// }
// 
// HttpBaseRequest::~HttpBaseRequest()
// {
// 	curl_easy_cleanup(curl);
// }
// 
// void HttpBaseRequest::Run()
// {
// 	Init();
// 
// 	int curl_code = curl_easy_perform(curl);
// 	if (curl_code == 28)
// 	{
// 		int retry_count = m_nRetry;
// 		while (retry_count > 0)
// 		{
// 			curl_code = curl_easy_perform(curl);
// 			if (curl_code != 28)
// 			{
// 				break;
// 			}
// 			retry_count--;
// 		}
// 	}
// 
// 
// }
// 
// void HttpBaseRequest::Init()
// {
// 	
// 	m_strMessage = "jsonStr={\"username\":\"1\",\"status\":\"1\",\"ticket\":\"1\",\"userid\":\"1\"}";
// 
// 	//	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);// 改协议头
// 	curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.135:9002/1/1001/qryLoginInfo");		//设置访问URL
// 																								//	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, "");
// 	curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_nOutTime);		// 由于设置传输时间URL
// 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, mRecvBuf); //将返回的http头输出到fp指向的文件
// 	curl_easy_setopt(curl, CURLOPT_HEADERDATA, mHeaderBuf); //将返回的html主体数据输出到fp指向的文件
// 	if (E_Requset_Type::e_Req_Post == m_eMsgType)
// 	{
// 		curl_easy_setopt(curl, CURLOPT_POST, 1);
// 		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_strMessage.c_str());
// 		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, m_strMessage.size());
// 	}
// 	else if (E_Requset_Type::e_Req_Get == m_eMsgType)
// 	{
// 
// 	}
// }
// 
// 
