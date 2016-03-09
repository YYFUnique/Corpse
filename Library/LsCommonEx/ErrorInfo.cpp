#include "stdafx.h"
#include "ErrorInfo.h"
#include <shlwapi.h>
#include <WinInet.h>
#include <Winsock2.h>

DWORD g_dwTlsIndex=TlsAlloc();
HANDLE g_hErrorLogFile=INVALID_HANDLE_VALUE;

void ReleaseProcessErrorInfo()
{
	ReleaseThreadErrorInfo();
	TlsFree(g_dwTlsIndex);
	if (g_hErrorLogFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hErrorLogFile);
		g_hErrorLogFile=INVALID_HANDLE_VALUE;
	}
}

void ReleaseThreadErrorInfo()
{

}

CString GetLastErrorInfo()
{
	CString szErrorInfo;
	LPVOID lpMsgBuf;

	DWORD dwErrorCode=GetLastError();
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,dwErrorCode,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL))
	{
		szErrorInfo.Format(_T("未知错误号0x%08X."),dwErrorCode);
		return szErrorInfo;
	}

	szErrorInfo=(LPCTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);// Free the buffer.

	return szErrorInfo;
}

ERROR_INFO& GetThreadErrorInfo()
{
	PERROR_INFO pErrorInfo=reinterpret_cast<PERROR_INFO>(TlsGetValue(g_dwTlsIndex));
	if (pErrorInfo == NULL) 
	{ 
		pErrorInfo = new ERROR_INFO;
		if (pErrorInfo != NULL) 
			TlsSetValue(g_dwTlsIndex, pErrorInfo);
	}
	return *pErrorInfo;
}

CString& GetThreadErrorInfoString()
{
	ERROR_INFO& ErrorInfo=GetThreadErrorInfo();

	return ErrorInfo.szErrorInfo;
}

DWORD SetErrorInfoV(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList)
{
	if (dwErrorType == KERNEL_ERROR && dwErrorCode != 0)
	{
		dwErrorCode = RtlNtStatusToDosError(dwErrorCode);
		dwErrorType = SYSTEM_ERROR;
	}

	if (dwErrorType == COM_ERROR)
	{
		dwErrorCode = HRESULT_CODE(dwErrorCode);
		dwErrorType = SYSTEM_ERROR;
	}

	if (dwErrorCode==0)
	{
		dwErrorCode=0xFFFFFFFF;
		if (dwErrorType==SYSTEM_ERROR)
			dwErrorCode = GetLastError();
		else if (dwErrorType==WINSOCK_ERROR)
		{
			dwErrorCode = WSAGetLastError();
			dwErrorType = SYSTEM_ERROR;
		}
	}

	ERROR_INFO& ErrorInfo=GetThreadErrorInfo();
	ErrorInfo.dwErrorCode=dwErrorCode;
	ErrorInfo.dwErrorType=dwErrorType;

	if (lpszErrorTitle)
		ErrorInfo.szErrorInfo.FormatV(lpszErrorTitle,argList);
	else
		ErrorInfo.szErrorInfo.Empty();

	if (dwErrorType==SYSTEM_ERROR)
	{
		//自动增加冒号
		if (!ErrorInfo.szErrorInfo.IsEmpty())
			if (ErrorInfo.szErrorInfo.Right(1).Compare(_T(":")))
				ErrorInfo.szErrorInfo+=_T(":");

		//当返回错误信息是RPC服务器不可用时，对该错误信息进行单独定义，以便用户理解。
		if (dwErrorCode==RPC_S_SERVER_UNAVAILABLE)
			ErrorInfo.szErrorInfo+=_T("后台服务未启动，建议您重启系统。如果重启系统后仍然不能正常工作，请您尝试重新安装客户端。");
		else if (dwErrorCode==RPC_S_INVALID_BOUND)
			ErrorInfo.szErrorInfo+=_T("后台服务程序版本不匹配，建议您重启系统。如果重启系统后仍然不能正常工作，请您尝试重新安装客户端。");
		else
		{
			HMODULE hMod = NULL;
			if (ErrorInfo.dwErrorCode>=12000 && ErrorInfo.dwErrorCode<=12174) 
				hMod = GetModuleHandle(_T("wininet.dll"));

			if (dwErrorCode != ERROR_INTERNET_EXTENDED_ERROR) 
			{
				DWORD dwFlag = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
				if (hMod)
					dwFlag |= FORMAT_MESSAGE_FROM_HMODULE; 

				LPVOID lpMsgBuf;
				if (!FormatMessage(dwFlag,hMod,dwErrorCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL))
				{
					CString szErrorInfo;
					szErrorInfo.Format(_T("未知系统错误0x%08X."),dwErrorCode);
					ErrorInfo.szErrorInfo+=szErrorInfo;
					return dwErrorCode;
				}

				ErrorInfo.szErrorInfo+=(LPCTSTR)lpMsgBuf;
				LocalFree(lpMsgBuf);
			}
			else 
			{
				TCHAR szExtendedErrorInfo[512];
				DWORD dwExtendedErrorCode, dwInfoBufferLen = _countof(szExtendedErrorInfo);
				InternetGetLastResponseInfo(&dwExtendedErrorCode, szExtendedErrorInfo, &dwInfoBufferLen);
				ErrorInfo.szErrorInfo += szExtendedErrorInfo;
				ErrorInfo.szErrorInfo.AppendFormat(_T("(错误号 %u)"), dwExtendedErrorCode);
			}

			ErrorInfo.szErrorInfo.TrimRight(_T("\r\n"));
		}
	}

	return dwErrorCode;
}


DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...)
{
	va_list paralist;
	va_start(paralist, lpszErrorTitle); 
	return SetErrorInfoV(dwErrorType,dwErrorCode,lpszErrorTitle,paralist);
}

DWORD SetErrorTitle(LPCTSTR lpszTitle,...)
{
	ERROR_INFO& ErrorInfo=GetThreadErrorInfo();
	int nIndex=-1;
	int nErrorInfoLen=ErrorInfo.szErrorInfo.GetLength()-1;
	LPCTSTR lpszErrorInfo=ErrorInfo.szErrorInfo;
	for (int i=0;i<nErrorInfoLen;i++)
	{
		if (lpszErrorInfo[i]!=':')
			continue;

		if (lpszErrorInfo[i+1]=='/' || lpszErrorInfo[i+1]=='\\' || (lpszErrorInfo[i+1]>='0' && lpszErrorInfo[i+1]<='9'))
			continue;

		nIndex=i;
		break;
	}

	CString szTitle;
	if (lpszTitle)
	{
		va_list paralist;
		va_start(paralist, lpszTitle); 
		szTitle.FormatV(lpszTitle,paralist);
	}
	szTitle.TrimRight(':');

	if (nIndex==-1)
		ErrorInfo.szErrorInfo = szTitle;
	else
		ErrorInfo.szErrorInfo=szTitle+CString((LPCTSTR)ErrorInfo.szErrorInfo+nIndex);

	return ErrorInfo.dwErrorCode;
}