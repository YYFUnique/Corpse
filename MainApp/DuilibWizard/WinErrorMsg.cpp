#include "stdafx.h"
#include "WinErrorMsg.h"
#include <WinInet.h>
#include <Winternl.h>
#pragma comment(lib,"Wininet.lib")

CWinErrorMsg* CWinErrorMsg::m_pLastErrorInfo = NULL;
const DWORD g_dwTlsIndex=TlsAlloc();

CWinErrorMsg* CWinErrorMsg::Init()
{
	if (m_pLastErrorInfo)
		return m_pLastErrorInfo;

	m_pLastErrorInfo = new CWinErrorMsg;
	return m_pLastErrorInfo;
}

void CWinErrorMsg::Release()
{
	PWIN_ERROR_INFO pErrorInfo=reinterpret_cast<PWIN_ERROR_INFO>(TlsGetValue(g_dwTlsIndex));
	if (pErrorInfo) 
	{
		delete pErrorInfo; 
		pErrorInfo = NULL;
	}
	TlsSetValue(g_dwTlsIndex, NULL);
}

CWinErrorMsg::CWinErrorMsg()
{

}

CWinErrorMsg::~CWinErrorMsg()
{

}

DWORD CWinErrorMsg::SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...)
{
	va_list paralist;
	va_start(paralist, lpszErrorTitle); 

// 	if (dwErrorType == KERNEL_ERROR && dwErrorCode != 0)
// 		dwErrorCode = RtlNtStatusToDosError(dwErrorCode);
// 	else 
	if (dwErrorType == COM_ERROR)
		dwErrorCode = HRESULT_CODE(dwErrorCode);
	
	if (dwErrorCode==0)
	{
		dwErrorCode=0xFFFFFFFF;
		if (dwErrorType==SYSTEM_ERROR)
			dwErrorCode = GetLastError();
		else if (dwErrorType==WINSOCK_ERROR)
			dwErrorCode = WSAGetLastError();
	}

	if (dwErrorType != CUSTOM_ERROR)
		dwErrorType = SYSTEM_ERROR;

	WIN_ERROR_INFO& WinErrorInfo = GetThreadErrorInfo();
	WinErrorInfo.dwErrorCode=dwErrorCode;
	WinErrorInfo.dwErrorType=dwErrorType;

	if (lpszErrorTitle)
		WinErrorInfo.strErrorInfo.Format(lpszErrorTitle,paralist);
	else
		WinErrorInfo.strErrorInfo.Empty();

	//自动增加冒号
	if (WinErrorInfo.strErrorInfo.IsEmpty() == FALSE)
	{
		WinErrorInfo.strErrorInfo.TrimRight(_T(":"));
		WinErrorInfo.strErrorInfo+=_T(":");
	}

	HMODULE hMod = NULL;
	LPVOID lpLastMsg = NULL;
	do 
	{
		if (WinErrorInfo.dwErrorCode>INTERNET_ERROR_BASE && WinErrorInfo.dwErrorCode<INTERNET_ERROR_LAST)
			hMod = GetModuleHandle(_T("wininet.dll"));

		if (dwErrorCode == ERROR_INTERNET_EXTENDED_ERROR)
		{
			TCHAR szExtendedErrorInfo[512];
			DWORD dwExtendedErrorCode, dwInfoBufferLen = _countof(szExtendedErrorInfo);
			InternetGetLastResponseInfo(&dwExtendedErrorCode, szExtendedErrorInfo, &dwInfoBufferLen);
			WinErrorInfo.strErrorInfo += szExtendedErrorInfo;
			WinErrorInfo.strErrorInfo.AppendFormat(_T("(错误号 %u)"), dwExtendedErrorCode);
		}
		
		DWORD dwFlag = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
		if (hMod)
			dwFlag |= FORMAT_MESSAGE_FROM_HMODULE; 
		
		if (FormatMessage(dwFlag,hMod,dwErrorCode,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR)&lpLastMsg,0,NULL) == FALSE)
		{
			WinErrorInfo.strErrorInfo.AppendFormat(_T("未知系统错误0x%08X."),dwErrorCode);
			return dwErrorCode;
		}

		WinErrorInfo.strErrorInfo+=(LPCTSTR)lpLastMsg;
		
	} while (FALSE);

	if (lpLastMsg)
		LocalFree(lpLastMsg);

	return dwErrorCode;
}

DWORD CWinErrorMsg::SetErrorTitle(LPCTSTR lpszTitle,...)
{
	WIN_ERROR_INFO& WinErrorInfo=GetThreadErrorInfo();

	int nItem = WinErrorInfo.strErrorInfo.ReverseFind(_T(':'));
	int nLen = WinErrorInfo.strErrorInfo.GetLength();
	CString strErrorInfoTitle;
	if (lpszTitle)
	{
		va_list paralist;
		va_start(paralist, lpszTitle); 
		strErrorInfoTitle.Format(lpszTitle,paralist);
	}
	strErrorInfoTitle.TrimRight(':');

	if (nItem==-1)
		WinErrorInfo.strErrorInfo = strErrorInfoTitle;
	else
		WinErrorInfo.strErrorInfo.Format(_T("%s%s"),strErrorInfoTitle,WinErrorInfo.strErrorInfo.Right(nLen - nItem));

	return WinErrorInfo.dwErrorCode;
}

CString& CWinErrorMsg::GetThreadErrorInfoString()
{
	WIN_ERROR_INFO& ErrorInfo = GetThreadErrorInfo();

	return ErrorInfo.strErrorInfo;
}

WIN_ERROR_INFO& CWinErrorMsg::GetThreadErrorInfo()
{
	PWIN_ERROR_INFO pErrorInfo=reinterpret_cast<PWIN_ERROR_INFO>(TlsGetValue(g_dwTlsIndex));
	if (pErrorInfo == NULL) 
	{ 
		pErrorInfo = new WIN_ERROR_INFO;
		if (pErrorInfo != NULL) 
			TlsSetValue(g_dwTlsIndex, pErrorInfo);
	}
	return *pErrorInfo;
}
