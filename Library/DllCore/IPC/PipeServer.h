#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API IMessageProcessor
{
public:
	virtual void ProcessMessage(LPCTSTR lpszRequest, CString& strResponse) = 0;
};

class DLL_API PipeServer
{
public:
	PipeServer();
	~PipeServer();

public:
	BOOL Listen(LPCTSTR lpszPipeName, IMessageProcessor* processor);
	void NotifyShutdown();
	BOOL IsListening() { return m_bListening; }

private:
	BOOL CreatePipe(LPCTSTR lpszPipeName);
	BOOL HandleConnectionOrTimeout(LPCTSTR lpszPipeName);
	void HandleConnection();

private:
	HANDLE m_hPipe;
	IMessageProcessor* m_pMessageProcessor;
	volatile BOOL m_bListening;
	volatile BOOL m_bGoingToStop;
};

