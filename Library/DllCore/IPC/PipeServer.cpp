#include "StdAfx.h"
#include "PipeServer.h"
#include "PipeCommon.h"
#include "DllCore/Log/LogHelper.h"
#include <Windows.h>


PipeServer::PipeServer()
	: m_hPipe(NULL),
	  m_pMessageProcessor(NULL),
	  m_bListening(FALSE),
	  m_bGoingToStop(FALSE)
{
}

PipeServer::~PipeServer()
{
}

BOOL PipeServer::Listen(LPCTSTR lpszPipeName, IMessageProcessor* processor)
{
	QLOG_INFO(_T("Main thread awaiting client connection on %s."), lpszPipeName);

	m_hPipe = INVALID_HANDLE_VALUE;
	m_pMessageProcessor = processor;
	m_bGoingToStop = FALSE;

	if (CreatePipe(lpszPipeName) == FALSE)
		return FALSE;

	m_bListening = TRUE;

	while (m_bGoingToStop == FALSE)
	{
		if (HandleConnectionOrTimeout(lpszPipeName) == FALSE)
		{
			QLOG_ERR(_T("Unable to establish pipe server."));
			m_bListening = FALSE;
			return FALSE;
		}
	}
	::CloseHandle(m_hPipe);
	QLOG_INFO(_T("Pipe server stopped."));

	m_bListening = FALSE;
	return TRUE;
}

long long _milliseconds_now()
{
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	} else {
		return GetTickCount();
	}
}

void PipeServer::NotifyShutdown()
{
	if (m_bListening == FALSE)
	{
		QLOG_INFO(_T("pipe server is not listening"));
		return;
	}

	QLOG_INFO(_T("Shuting down pipe server."));

	m_bGoingToStop = FALSE;

	return;
}

BOOL PipeServer::CreatePipe(LPCTSTR lpszPipeName)
{
	SECURITY_ATTRIBUTES sa;
	sa.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
	// ACL is set as NULL in order to allow all access to the object.
	SetSecurityDescriptorDacl(sa.lpSecurityDescriptor, TRUE, NULL, FALSE);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	CString strPipeName;
	strPipeName.Format(_T("\\\\.\\pipe\\%s"), lpszPipeName);
	m_hPipe = ::CreateNamedPipe(
														strPipeName,        // pipe name
														PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,      // read/write access
														PIPE_TYPE_MESSAGE |       // message type pipe
														PIPE_READMODE_MESSAGE |   // message-read mode
														PIPE_WAIT,                // blocking mode
														PIPE_UNLIMITED_INSTANCES, // max. instances
														PipeCommon::kBufferSize,  // output buffer size
														PipeCommon::kBufferSize,  // input buffer size
														PipeCommon::kPipeTimeout, // client time-out
														&sa);                    // default security attribute

	free(sa.lpSecurityDescriptor);
	sa.lpSecurityDescriptor = NULL;

	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		QLOG_ERR(_T("CreateNamedPipe failed, error=%d."), ::GetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL PipeServer::HandleConnectionOrTimeout(LPCTSTR lpszPipeName)
{
	OVERLAPPED overlapped = {0, 0, 0, 0, NULL};
	overlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (overlapped.hEvent == NULL)
	{
		QLOG_ERR(_T("Fail to create event, Win32Err: %d"), GetLastError());
		return FALSE;
	}

	BOOL bSuccess = FALSE;
	if (::ConnectNamedPipe(m_hPipe, &overlapped) == FALSE)
	{
		switch (::GetLastError())
		{
			case ERROR_PIPE_CONNECTED:
				HandleConnection();
				bSuccess = TRUE;
				break;
			case ERROR_IO_PENDING:
				switch (::WaitForSingleObject(overlapped.hEvent, PipeCommon::kPipeTimeout))
				{
					case WAIT_OBJECT_0:
					{
						DWORD dwTransfered;
						if (::GetOverlappedResult(overlapped.hEvent, &overlapped, &dwTransfered, FALSE))
						{
							HandleConnection();
							bSuccess = TRUE;
						}
						else
						{
							QLOG_ERR(_T("GetOverlappedResult failed, error=%d."), ::GetLastError());
							bSuccess = FALSE;
						}
						break;
					}
					case WAIT_TIMEOUT:
						::CancelIo(m_hPipe);
						bSuccess = TRUE;
						break;
					default:
						QLOG_ERR(_T("WaitForSingleObject failed, error=%d."), ::GetLastError());
						bSuccess = FALSE;
						break;
				}
				break;
			default:
				QLOG_ERR(_T("ConnectNamedPipe failed, error=%d."), ::GetLastError());
				bSuccess = FALSE;
				break;
		}
	}
	::CloseHandle(overlapped.hEvent);
	return bSuccess;
}

void PipeServer::HandleConnection()
{
	QLOG_ERR(_T("Handling connection on pipe %08x."), m_hPipe);

	CString strRequest;
	if (PipeCommon::Receive(m_hPipe, strRequest) == FALSE)
	{
		QLOG_ERR(_T("Unable to receive request."));
		::DisconnectNamedPipe(m_hPipe);
		return;
	}

	if (m_pMessageProcessor != NULL)
	{
		CString strResponse;
		m_pMessageProcessor->ProcessMessage(strRequest, strResponse);
		if (!PipeCommon::Send(m_hPipe, strResponse))
		{
			QLOG_ERR(_T("Unable to send response."));
			::DisconnectNamedPipe(m_hPipe);
			return;
		}
	}
	::FlushFileBuffers(m_hPipe);
	::DisconnectNamedPipe(m_hPipe);
}
