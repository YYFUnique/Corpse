#pragma once
#include <string>

namespace PipeCommon
{
	const DWORD kPipeTimeout = 1000;
	const DWORD kBufferSize = 4096;
	const UINT kConnectRetry = 5;

	BOOL Receive(HANDLE hPipe, CString& strMsg);
	BOOL Send(HANDLE hPipe, LPCTSTR lpszMsg);
}
