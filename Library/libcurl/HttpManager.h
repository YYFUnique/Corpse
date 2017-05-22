#pragma once

#include "HTTPRequest.h"
#include "HttpThreadPool.h"

#ifdef LS_STATIC_LIB_CALL
#define LIB_CURL_API 
#elif defined  CURL_EXPORTS
#define LIB_CURL_API __declspec(dllexport)
#else
#define LIB_CURL_API __declspec(dllimport)
#endif

class LIB_CURL_API CHTTPManager
{
public:
	CHTTPManager();
	~CHTTPManager();

	BOOL Init();

	void SetLibcurlCallback(ILibcurlResponse* pCallback);

	BOOL doHttpGet(DWORD dwEvent, LPCTSTR lpszURL, LPCTSTR lpszData, DWORD dwTimeout =3);

	BOOL doHttpPost(DWORD dwEvent, LPCTSTR lpszURL, LPCTSTR lpszData, DWORD dwTimeout =3);

private:
	HttpThreadPool* m_pThreadPool;
	CHTTPRequest*	m_pHttpRequest;
	ILibcurlResponse* m_pLibcurlCallback;
};


