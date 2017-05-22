#pragma once

#include "libcurl/curl/curl.h"
#include <atlstr.h>

#ifdef LS_STATIC_LIB_CALL
#define LIB_CURL_API 
#elif defined  CURL_EXPORTS
#define LIB_CURL_API __declspec(dllexport)
#else
#define LIB_CURL_API __declspec(dllimport)
#endif

class IHttpRequest
{
public:
	virtual void Run() = 0;
};

class ILibcurlResponse
{
public:
	virtual int ProgressHeader(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb) = 0;
	virtual int ProgressBody(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb) = 0;
};

class LIB_CURL_API CHTTPRequest : public IHttpRequest
{
public:
	CHTTPRequest();
	~CHTTPRequest();

	virtual void Run();
	BOOL Initialize(DWORD dwEvent, LPCTSTR lpszURL, LPCTSTR lpszData);
	//virtual void Init();
	//处理Http请求完成
	virtual void OnComplete(CURLcode code);

	void SetRetry(DWORD dwRetry);
	void SetTimeout(DWORD dwTimeout);
	void SetCallback(ILibcurlResponse* pCallback);
	BOOL SetUserAgent(LPCTSTR lpAgent);

protected:
	static size_t ProcessFunc(LPVOID lpData, size_t size, size_t nmemb, LPVOID lParam);

protected:
	CURL*					m_pCurl;
	DWORD				m_dwEvent;
	DWORD				m_dwRetry;
	DWORD				m_dwTimeout;
	ILibcurlResponse* m_pCallback;
};

