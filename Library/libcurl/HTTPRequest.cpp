 #include "stdafx.h"
#include "HTTPRequest.h"

typedef struct LPARAMINFO
{
	CHTTPRequest*  pLibcurl;
	DWORD	 dwEvent;
}LPARAMINFO,*PLPARAMINFO;

CHTTPRequest::CHTTPRequest()
{
	m_pCallback = NULL;
}

CHTTPRequest::~CHTTPRequest()
{
	if (m_pCurl != NULL)
	{
		curl_easy_cleanup(m_pCurl);
		m_pCurl = NULL;
	}
}

BOOL CHTTPRequest::Initialize(DWORD dwEvent, LPCTSTR lpszURL, LPCTSTR lpszData)
{
	BOOL bSuccess = FALSE;
	do 
	{
		m_pCurl = curl_easy_init();
		if (m_pCurl == NULL)
			break;
		
		CString strHttpUrl(lpszURL);

		if (lpszData != NULL)
		{
			strHttpUrl.TrimRight(_T("?"));
			strHttpUrl.AppendFormat(_T("?%s"), lpszData);
		}

		m_dwEvent = dwEvent;
		::curl_easy_setopt(m_pCurl, CURLOPT_URL, CStringA(strHttpUrl));
		::curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1 );
		::curl_easy_setopt(m_pCurl, CURLOPT_HEADER, 0);										//不使用头
		::curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, m_dwTimeout);

	} while (FALSE);

	return bSuccess;
}

void CHTTPRequest::Run()
{
	if (m_pCurl == NULL)
		return;

	::curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, CHTTPRequest::ProcessFunc);

	LPARAMINFO lParamInfo;
	lParamInfo.pLibcurl = this;
	lParamInfo.dwEvent = m_dwEvent;

	::curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &lParamInfo);

	CURLcode Curlcode = curl_easy_perform(m_pCurl);
	if (Curlcode == CURLE_OPERATION_TIMEDOUT)
	{
		int  nRetryCount = m_dwRetry;
		while (nRetryCount > 0)
		{
			Curlcode = curl_easy_perform(m_pCurl);
			if (Curlcode != CURLE_OPERATION_TIMEDOUT)
				break;

			--nRetryCount;
		}
	}

	OnComplete(Curlcode);
}

void CHTTPRequest::OnComplete(CURLcode Curlcode)
{
	delete this;
}

void CHTTPRequest::SetCallback(ILibcurlResponse* pCallback)
{
	m_pCallback = pCallback;
}

BOOL CHTTPRequest::SetUserAgent(LPCTSTR lpszAgent)
{
	if (lpszAgent == NULL)
		return FALSE;

	int nLen = _tcslen(lpszAgent);
	if (nLen == 0)
		return FALSE;

	return CURLE_OK == curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, lpszAgent);
}

void CHTTPRequest::SetRetry(DWORD dwRetry)
{
	m_dwRetry = dwRetry;
}

void CHTTPRequest::SetTimeout(DWORD dwTimeout)
{
	m_dwTimeout = dwTimeout;
}

size_t CHTTPRequest::ProcessFunc(LPVOID lpData, size_t size, size_t nmemb, LPVOID lParam)
{
	size_t nSizeRet = size * nmemb;
	LPARAMINFO* plParamInfo = (LPARAMINFO*)lParam;
	if (plParamInfo == NULL)
		return nSizeRet;

	if (plParamInfo->pLibcurl->m_pCallback != NULL)
		nSizeRet = plParamInfo->pLibcurl->m_pCallback->ProgressBody(plParamInfo->dwEvent, lpData, size, nmemb);

	return nSizeRet;
}