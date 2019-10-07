#pragma once

#ifdef LS_STATIC_LIB_CALL
#define MODULE_PLATFORM_API 
#elif defined  MODULE_PLATFORM_EXPORTS
#define MODULE_PLATFORM_API __declspec(dllexport)
#else
#define MODULE_PLATFORM_API __declspec(dllimport)
#endif

#pragma warning (disable : 4251)
#include "DllCore/IPC/PipeServer.h"
typedef enum tagCommunicationNodeType
{
	UNKNOWN_NODE = 0,
	CORE_NODE = 1,
	UI_NODE = 2,
	STCMD_NODE = 3,
	LAUNCH_NODE = 4,
	BOOT_NODE = 5,
	PATCH_AGENT_NODE = 6,
	STCLIENT_NODE = 7,
	SUPPORT_NODE = 8,
	SDT_NODE = 9,
	APPLY_HOTFIX_NODE = 10,
	STAGENT_NODE = 11,
	MPCORE_NODE = 12,
	SDT_AGENT_NODE = 13
}COMMUNICATION_NODE;

class MODULE_PLATFORM_API IRequestHandler
{
public:
	virtual BOOL HandleRequest(LPCTSTR lpszRequest, CString& strResponse) = 0;
};

class MODULE_PLATFORM_API CommunicationCenter : public IMessageProcessor
{
public:
	CommunicationCenter();
	virtual ~CommunicationCenter();

	void Init(const COMMUNICATION_NODE& node, LPCTSTR lpszPipePrefix, IRequestHandler* iHandler);
	BOOL RunAsync();
	void Stop();
	BOOL Request(const COMMUNICATION_NODE& node, LPCTSTR lpszRequest, CString& strResponse, LPDWORD dwErrorCode = NULL);
	BOOL IsRunning();

	void SetPipePrefix(LPCTSTR lpszPipePrefix)
	{
		m_strPipePrefix = lpszPipePrefix;
	}

	virtual void ProcessMessage(LPCTSTR lpszPipePrefix, CString& strResponse);

private:
	static UINT CALLBACK RunEntry(void* instance);
	static CString GetPipeName(const COMMUNICATION_NODE& node, LPCTSTR lpszPipePrefix);
	static BOOL Encrypt(LPCTSTR lpszPlain, CString& strCipher);
	static BOOL Decrypt(LPCTSTR lpszCipher, CString& strPlain);

	BOOL Run();

private:
	BOOL m_Running;
	HANDLE m_hThread;
	DWORD m_nWorkerTID;
	CString m_strPipePrefix;
	PipeServer m_PipeServer;
	COMMUNICATION_NODE m_Node;
	IRequestHandler* m_pRequestHandler;
};