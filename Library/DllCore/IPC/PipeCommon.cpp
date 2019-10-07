#include "StdAfx.h"
#include "PipeCommon.h"
#include "DllCore/Log/LogHelper.h"
#include <sstream>

#include <Windows.h>

namespace PipeCommon
{
	BOOL Receive(HANDLE hPipe, CString& strMsg)
	{
		QLOG_DBG(_T("Receiving message."));
		DWORD dwRead = 0;
		CHAR bData[PipeCommon::kBufferSize];
		BOOL bSuccess = FALSE;
		std::stringstream message_stream;
		while (TRUE)
		{ 
			// Read client requests from the pipe.
			bSuccess = ::ReadFile(
											hPipe,                                   // handle to pipe 
											bData,                                 // buffer to receive data 
											PipeCommon::kBufferSize * sizeof(CHAR),   // size of buffer 
											&dwRead,                            // number of bytes read 
											NULL);                                  // not overlapped I/O 

			if (bSuccess || ::GetLastError() == ERROR_MORE_DATA)
			{
				QLOG_DBG(_T("Received %d bytes message."), dwRead);
				message_stream.write(bData, dwRead / sizeof(CHAR));
				if (bSuccess)
				{
					QLOG_DBG(_T("Finished receiving data."));
					break;
				}
			}
			else
			{   
				if (::GetLastError() == ERROR_BROKEN_PIPE)
				{
					QLOG_ERR(_T("Client disconnected."));
					break;
				}
				else
				{
					QLOG_ERR(_T("ReadFile failed, error=%d."), ::GetLastError());
					return false;
				}
			}
		}
		
		strMsg.Format(_T("%s"), CString(message_stream.str().c_str()));
		return TRUE;
	}

	BOOL Send(HANDLE hPipe, LPCTSTR lpszMsg)
	{
		QLOG_DBG(_T("Sending message."));
		CStringA strMsg = CStringA(lpszMsg);
		DWORD dwBytesToWrite = static_cast<DWORD>(strMsg.GetLength()) * sizeof(BYTE);
		DWORD dwBytesWrite = 0;
		QLOG_DBG(_T("Sending %d byte message."), dwBytesToWrite);

		BOOL bSuccess = ::WriteFile(
													hPipe,                      // handle to pipe
													strMsg,           // buffer to write from
													dwBytesToWrite,            // number of bytes to write
													&dwBytesWrite,            // number of bytes written
													NULL);                     // not overlapped I/O

		if (!bSuccess || dwBytesToWrite != dwBytesWrite)
		{   
			QLOG_ERR(_T("WriteFile failed, error=%d."), ::GetLastError()); 
			return FALSE;
		}
		return TRUE;
	}
}