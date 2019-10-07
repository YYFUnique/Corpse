#include "StdAfx.h"
#include "PipeClient.h"
#include "PipeCommon.h"
#include "DllCore/Log/LogHelper.h"
#include <Windows.h>

PipeClient::PipeClient()
	: m_hPipe(INVALID_HANDLE_VALUE)
{
}

PipeClient::~PipeClient()
{
	this->Disconnect();
}

BOOL PipeClient::Connect(LPCTSTR lpszPipeName, LPDWORD dwErrorCode /* = NULL */)
{
	DWORD dwError = ERROR_SUCCESS;
	DWORD *pErrorCode = (dwErrorCode == NULL) ? &dwError : dwErrorCode;
	*pErrorCode = ERROR_SUCCESS;
	
	if (m_hPipe != INVALID_HANDLE_VALUE)
		this->Disconnect();

	CString strPipeName;
	strPipeName.Format(_T("\\\\.\\pipe\\%s"), lpszPipeName);
	QLOG_APP(_T("Connecting to %s."), strPipeName);

	for (UINT nRetry = 0; nRetry < PipeCommon::kConnectRetry; ++nRetry)
	{
		m_hPipe = ::CreateFile(
											strPipeName,   // pipe name
											GENERIC_READ | GENERIC_WRITE,       // read and write access
											0,                   // no sharing
											NULL,                // default security attributes
											OPEN_EXISTING,       // opens existing pipe
											0,                   // default attributes
											NULL);               // no template file

		if (m_hPipe != INVALID_HANDLE_VALUE)
		{
			DWORD dwMode = PIPE_READMODE_MESSAGE;
			BOOL bSuccess = ::SetNamedPipeHandleState(
																						m_hPipe,   // pipe handle
																						&dwMode,    // new pipe mode
																						NULL,     // don't set maximum bytes
																						NULL);    // don't set maximum time
			if (bSuccess == FALSE) 
			{
				QLOG_ERR(_T("SetNamedPipeHandleState failed, error=%d."), ::GetLastError());
				this->Disconnect();
				return FALSE;
			}
			return true;
		}
		QLOG_APP(_T("Pipe is not ready, retry=%d."), nRetry);

		*pErrorCode = ::GetLastError();
		if (*pErrorCode == ERROR_PIPE_BUSY)
		{
			if (::WaitNamedPipe(strPipeName, PipeCommon::kPipeTimeout) == FALSE)
			{
				QLOG_WAR(_T("WaitNamedPipe timed out."));
			}
		}
		else if (*pErrorCode == ERROR_FILE_NOT_FOUND)
		{
			// Wait kPipeTimeout seconds for pipe server to be created
			::Sleep(PipeCommon::kPipeTimeout);
		}
		else
		{
			QLOG_WAR(_T("Could not open pipe, error=%d."), GetLastError()); 
			::Sleep(PipeCommon::kPipeTimeout);
		}
	}
	QLOG_ERR(_T("Unable to connect to pipe server."));
	return FALSE;
}

BOOL PipeClient::MakeRequest(LPCTSTR lpszRequest, CString& strResponse)
{
	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		QLOG_ERR(_T("Pipe is not connected."));
		return false;
	}

	if (PipeCommon::Send(m_hPipe, lpszRequest) == FALSE)
	{
		return FALSE;
	}

	if (PipeCommon::Receive(m_hPipe, strResponse) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

void PipeClient::Disconnect()
{
	if (m_hPipe != INVALID_HANDLE_VALUE && m_hPipe != NULL)
	{
		::DisconnectNamedPipe(m_hPipe);
		::CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
}