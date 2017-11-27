#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LIB_CURL_API 
#elif defined  CURL_EXPORTS
#define LIB_CURL_API __declspec(dllexport)
#else
#define LIB_CURL_API __declspec(dllimport)
#endif

#include "libcurl.h"
#include "libcurl/curl/curl.h"
#include "libuv/uv.h"
 
#include <atlstr.h>

typedef struct curl_context_s {
	uv_poll_t			hPoll;
	curl_socket_t	CurlSocketFd;
	CURLM*			pCURLM;
}CurlContext;

class LIB_CURL_API CMultiRequest
{
public:
	CMultiRequest();
	~CMultiRequest();

public:
	void Init();
	
	void Stop();
	void SetCallback(ILibcurlCallback* pCallback);
	CurlContext* CrateCurlContext(curl_socket_t CurlSockFd);
	BOOL doHttpGet(DWORD dwEvent, LPCTSTR lpszURL);
protected:
	static size_t ProcessFunc(LPVOID lpData, size_t size, size_t nm, LPVOID lParam);
protected:
	//check_multi_info
	
	static void _uvClose(uv_handle_t* handle);
	static void CheckMultiInfo(CURLM* pCURLM);
	static void doSocketAction(uv_poll_t* uvPollData, int status, int nEvents);
	static void OnTimeout(uv_timer_t *hTimer);
	static UINT workThread(LPVOID lParam);
	static UINT TimeFunc(CURLM* pMulti, LONG nTime, LPVOID lParam);
	static UINT SocketFunc(CURL* pEasy, curl_socket_t CurlSocket, int nAction, LPVOID lParam, LPVOID socketp);
private:
	uv_loop_t*			m_pLoop;
	uv_timer_t			m_Timeout;
	uv_async_t			m_hAsync;

	//curl_multi���
	CURLM*				m_pCURLM;
	
	HANDLE				m_hEvent;			// �����¼�������httpGet����źŽ����ڼ���״̬
	//�ź���
	HANDLE				m_hSema;			// �ź����������û��߽���Դ�������
	
	BOOL					m_bExist;
	DWORD				m_dwTimeout;
	//libcurl�ص�������
	ILibcurlCallback* m_pCallback;
};