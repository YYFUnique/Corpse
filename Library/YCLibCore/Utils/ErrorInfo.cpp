#include "stdafx.h"
#include "ErrorInfo.h"
#include <shlwapi.h>
#include <WinInet.h>

#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Ws2_32.lib")
/*
����˵��:���ڴ˴���Ϊ�˽����Ϊ�ⲿ�������乹�캯������֮ǰ��CPP��
��ĳЩȫ�ֱ���δ��ʼ�������µ��÷Ƿ��Ĵ���,�������ⲿ����֮ǰ����
��ʼ����CPP�е�ȫ�ֱ���
*/
#ifdef LS_STATIC_LIB_CALL
#pragma warning(disable:4073)
#pragma init_seg(lib)
#endif

DWORD g_dwTlsIndex = TlsAlloc();

void ReleaseThreadErrorInfo()
{
	PERROR_INFO pErrorInfo=reinterpret_cast<PERROR_INFO>(TlsGetValue(g_dwTlsIndex));
	if (pErrorInfo) 
		delete pErrorInfo; 
	TlsSetValue(g_dwTlsIndex, NULL);
}

void ReleaseProcessErrorInfo()
{
	ReleaseThreadErrorInfo();
	TlsFree(g_dwTlsIndex);
}

CString GetLastErrorInfo()
{
	CString szErrorInfo;
	LPVOID lpMsgBuf;

	DWORD dwErrorCode=GetLastError();
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,dwErrorCode,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL))
	{
		szErrorInfo.Format(_T("δ֪�����0x%08X."),dwErrorCode);
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

BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode)
{
	ERROR_INFO& ErrorInfo=GetThreadErrorInfo();
	if (ErrorInfo.dwErrorType != dwErrorType)
		return FALSE;
	else if (dwErrorType == SYSTEM_ERROR)
		return ErrorInfo.dwErrorCode == dwErrorCode;
	else if (dwErrorType == CUSTOM_ERROR)
		if (dwErrorCode == 0xffffffff)
			return TRUE;
		else
			return ErrorInfo.dwErrorCode == dwErrorCode;

	return FALSE;
}

void RtlNtStatusToDosError(DWORD& dwErrorCode)
{
	HMODULE hModule = NULL;
	do 
	{
		hModule = LoadLibrary(_T("ntdll.dll"));
		if (hModule == NULL)
			break;

		LPFN_RtlDosErrorFromNtStatus lpRtlNtStatusToDosError = NULL;
		lpRtlNtStatusToDosError = (LPFN_RtlDosErrorFromNtStatus)GetProcAddress(hModule,"RtlNtStatusToDosError");
		ULONG ulRet = lpRtlNtStatusToDosError(dwErrorCode);

		dwErrorCode = ulRet;
	} while (FALSE);
	
	if (hModule != NULL)
		FreeLibrary(hModule);
}

DWORD SetErrorInfoV(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList)
{
	if (dwErrorType == KERNEL_ERROR && dwErrorCode != 0)
	{
		//dwErrorCode = RtlDosErrorFromNtStatus() 
		RtlNtStatusToDosError(dwErrorCode);
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
		//�Զ�����ð��
		if (!ErrorInfo.szErrorInfo.IsEmpty())
			if (ErrorInfo.szErrorInfo.Right(1).Compare(_T(":")))
				ErrorInfo.szErrorInfo+=_T(":");

		//�����ش�����Ϣ��RPC������������ʱ���Ըô�����Ϣ���е������壬�Ա��û���⡣
		if (dwErrorCode==RPC_S_SERVER_UNAVAILABLE)
			ErrorInfo.szErrorInfo+=_T("��̨����δ����������������ϵͳ���������ϵͳ����Ȼ�������������������������°�װ�ͻ��ˡ�");
		else if (dwErrorCode==RPC_S_INVALID_BOUND)
			ErrorInfo.szErrorInfo+=_T("��̨�������汾��ƥ�䣬����������ϵͳ���������ϵͳ����Ȼ�������������������������°�װ�ͻ��ˡ�");
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
					szErrorInfo.Format(_T("δ֪ϵͳ����0x%08X."),dwErrorCode);
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
				ErrorInfo.szErrorInfo.AppendFormat(_T("(����� %u)"), dwExtendedErrorCode);
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

void ResetErrorInfo()
{
	ERROR_INFO& ErrorInfo=GetThreadErrorInfo();
	ErrorInfo.dwErrorCode = ErrorInfo.dwErrorType =0;
	ErrorInfo.szErrorInfo.Empty();
}

void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...)
{
	ERROR_INFO& ErrorInfo=GetThreadErrorInfo();

	CString strErrorInfo = ErrorInfo.szErrorInfo;

	va_list paralist;
	va_start(paralist, lpszErrorTitle); 
	SetErrorInfoV(dwErrorType,dwErrorCode,lpszErrorTitle,paralist);

	if (strErrorInfo.IsEmpty() == FALSE)
		ErrorInfo.szErrorInfo = strErrorInfo + _T("\r\n") + ErrorInfo.szErrorInfo;
}

CString GetLogModuleFileName()
{
	static CString s_strLogFileName;
	if (s_strLogFileName.IsEmpty())
	{
		/*InitializeCriticalSection(&g_csWriteFileMutex);*/

		TCHAR szExeTitle[MAX_PATH];
		::GetModuleFileName(0,szExeTitle,MAX_PATH);
		PathRemoveExtension(szExeTitle);
		s_strLogFileName = PathFindFileName(szExeTitle);
	}
	return s_strLogFileName;
}