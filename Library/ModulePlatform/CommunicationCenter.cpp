#include "StdAfx.h"
#include "CommunicationCenter.h"

#include "DllCore/IPC/PipeClient.h"
#include "DllCore/Log/LogHelper.h"

CommunicationCenter::CommunicationCenter()
	: m_Node(UNKNOWN_NODE),
	  m_pRequestHandler(NULL),
	  m_strPipePrefix(_T("")),
	  m_hThread(NULL),
	  m_nWorkerTID(0),
	  m_Running(FALSE)
{
}

CommunicationCenter::~CommunicationCenter()
{
	if (this->IsRunning())
		this->Stop();
}

void CommunicationCenter::Init(const COMMUNICATION_NODE &node, LPCTSTR lpszPipePrefix, IRequestHandler *handler)
{
	m_Node = node;
	m_strPipePrefix = lpszPipePrefix;
	m_pRequestHandler = handler;
}

BOOL CommunicationCenter::RunAsync()
{
	if (m_Running)
	{
		QLOG_WAR(_T("Already running, skipped."));
		return FALSE;
	}

	m_hThread = CreateThread(
												NULL,           // no security attribute
												0,              // default stack size
												(LPTHREAD_START_ROUTINE)RunEntry,       // thread proc
												this,           // thread parameter
												0,              // not suspended
												&m_nWorkerTID);    // returns thread ID
	
	if (m_hThread == NULL)
	{
		QLOG_ERR(_T("CreateThread failed %d."), GetLastError());
		return FALSE;
	}
	QLOG_APP(_T("Thread created, id=%08x."), m_nWorkerTID);
	
	const UINT kRetryMax = 1000;
	const UINT kRetryInterval = 10;
	UINT nRetry = 0;
	while (!m_Running && ++nRetry < kRetryMax)
	{
		::Sleep(kRetryInterval);
	}

	if (!m_Running)
	{
		QLOG_ERR(_T("Unable to create communication center in %d retries."), kRetryMax);
		return FALSE;
	}
	return TRUE;
}

void CommunicationCenter::Stop()
{
	QLOG_INFO(_T("stopping"));

	if (this->IsRunning() == FALSE)
	{
		QLOG_INFO(_T("already stopped"));
		return;
	}

	m_PipeServer.NotifyShutdown();

	if (::GetCurrentThreadId() != m_nWorkerTID) // we cannot wait ourself...
	{
		DWORD dwRet = WAIT_TIMEOUT;
		int nRetry = 10;
		do
		{
			dwRet = ::WaitForSingleObject(reinterpret_cast<HANDLE>(m_hThread), 1000);
			--nRetry;
		} while (dwRet == WAIT_TIMEOUT && nRetry > 0);

		if (dwRet == WAIT_TIMEOUT)
		{
			QLOG_FATAL(_T("timeout, forcedly terminate the worker thread"));
			::TerminateThread(reinterpret_cast<HANDLE>(m_hThread), 0);
		}
		::CloseHandle(reinterpret_cast<HANDLE>(m_hThread));
		m_nWorkerTID = 0;
		m_hThread = NULL;
	}

	QLOG_INFO(_T("stopped"));
}

BOOL CommunicationCenter::Request(const COMMUNICATION_NODE& node, LPCTSTR lpszRequest, CString& strResponse, LPDWORD dwErrorCode /* = NULL */)
{
	QLOG_DBG(_T("Connecting node %d."), node);
	PipeClient client;
	if (client.Connect(GetPipeName(node, m_strPipePrefix), dwErrorCode) == FALSE)
	{
		QLOG_ERR(_T("Unable to connect node %d."), node);
		return FALSE;
	}

	CString strEncryptedRequest;
	if (Encrypt(lpszRequest, strEncryptedRequest) == FALSE)
	{
		QLOG_ERR(_T("Unable to encrypt request string."));
		return FALSE;
	}
	
	CString strEncryptedResponse;
	if (client.MakeRequest(strEncryptedRequest, strEncryptedResponse) == FALSE)
	{
		QLOG_ERR(_T("Unable to make request to node %d."), node);
		return FALSE;
	}

	if (Decrypt(strEncryptedResponse, strResponse) == FALSE)
	{
		QLOG_ERR(_T("Unable to decrypt response string."));
		return FALSE;
	}

	client.Disconnect();
	return TRUE;
}

BOOL CommunicationCenter::IsRunning()
{
	return m_Running;
}

void CommunicationCenter::ProcessMessage(LPCTSTR lpszRequest, CString& strResponse)
{
	strResponse.Empty();
	CString strDecryptedRequest;
	if (Decrypt(lpszRequest, strDecryptedRequest) == FALSE)
	{
		QLOG_ERR(_T("Unable to decrypt request string."));
		return;
	}

	CString strPlainResponse;
	if (!m_pRequestHandler->HandleRequest(strDecryptedRequest, strPlainResponse))
	{
		QLOG_ERR(_T("Unable to handle request."));
	}

	if (Encrypt(strPlainResponse, strResponse) == FALSE)
	{
		QLOG_ERR(_T("Unable to encrypt response string."));
		return;
	}
}

UINT CommunicationCenter::RunEntry(void* instance)
{
	if (static_cast<CommunicationCenter*>(instance)->Run() == FALSE)
		return 0;
	else
		return 1;
}

CString CommunicationCenter::GetPipeName(const COMMUNICATION_NODE &node, LPCTSTR lpszPipePrefix)
{
	CString strPipeName = lpszPipePrefix;
	
	switch (node)
	{
		case CORE_NODE:
			strPipeName += _T("@@@TMPS_ST_CORE@@@");
			break;
		case STCMD_NODE:
			strPipeName += _T("@@@TMPS_ST_CMD@@@");
			break;
		case UI_NODE:
			strPipeName += _T("@@@TMPS_ST_UI@@@");
			break;
		case LAUNCH_NODE:
			strPipeName += _T("@@@TMPS_ST_LAUNCH@@@");
			break;
		case BOOT_NODE:
			strPipeName += _T("@@@TMPS_ST_BOOT@@@");
			break;
		case SUPPORT_NODE:
			strPipeName += _T("@@@TMPS_ST_SUPPORT@@@");
			break;
		case SDT_NODE:
			strPipeName += _T("@@@TMPS_ST_SDT@@@");
			break;
		case APPLY_HOTFIX_NODE:
			strPipeName += _T("@@@TMPS_ST_APPLY_HOTFIX@@@");
			break;
		case STAGENT_NODE:
			strPipeName = _T("@@@TMPS_ST_STAGENT@@@");
			break;
		case MPCORE_NODE:
			strPipeName = _T("@@@TMPS_MP_CORE@@@");
			break;
		case SDT_AGENT_NODE:
			strPipeName = _T("@@@TMPS_ST_SDT_AGENT@@@");
			break;
		default:
			strPipeName += _T("@@@TMPS_UNKNOWN@@@");
			break;
	}

	return strPipeName;
}

BOOL CommunicationCenter::Encrypt(LPCTSTR lpszPlain, CString& strCipher)
{
	// TODO(zi): implementation
	strCipher = lpszPlain;
	return TRUE;
}

BOOL CommunicationCenter::Decrypt(LPCTSTR lpszCipher, CString& strPlain)
{
	// TODO(zi): implementation
	strPlain = lpszCipher;
	return TRUE;
}

BOOL CommunicationCenter::Run()
{
	m_Running = TRUE;
	if (m_PipeServer.Listen(GetPipeName(m_Node, m_strPipePrefix), this) == FALSE)
	{
		QLOG_ERR(_T("Unable to create pipe server."));
		m_Running = FALSE;
		return FALSE;
	}
	m_Running = FALSE;
	return TRUE;
}
