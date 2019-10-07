#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API PipeClient
{
public:
	PipeClient();
	~PipeClient();

public:
	BOOL Connect(LPCTSTR lpszPipeName, LPDWORD dwErrorCode = NULL);
	BOOL MakeRequest(LPCTSTR lpszRequest, CString& strResponse);
	void Disconnect();	

private:
	HANDLE m_hPipe;
};
