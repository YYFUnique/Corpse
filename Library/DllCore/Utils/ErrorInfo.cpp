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
	CString strErrorInfo;
	LPVOID lpMsgBuf;

	DWORD dwErrorCode=GetLastError();
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,dwErrorCode,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL))
	{
		strErrorInfo.Format(_T("δ֪�����0x%08X."),dwErrorCode);
		return strErrorInfo;
	}

	strErrorInfo=(LPCTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);// Free the buffer.

	return strErrorInfo;
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

	return ErrorInfo.strErrorInfo;
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
		ErrorInfo.strErrorInfo.FormatV(lpszErrorTitle,argList);
	else
		ErrorInfo.strErrorInfo.Empty();

	if (dwErrorType==SYSTEM_ERROR)
	{
		//�Զ�����ð��
		if (!ErrorInfo.strErrorInfo.IsEmpty())
			if (ErrorInfo.strErrorInfo.Right(1).Compare(_T(":")))
				ErrorInfo.strErrorInfo+=_T(":");

		//�����ش�����Ϣ��RPC������������ʱ���Ըô�����Ϣ���е������壬�Ա��û���⡣
		if (dwErrorCode==RPC_S_SERVER_UNAVAILABLE)
			ErrorInfo.strErrorInfo+=_T("��̨����δ����������������ϵͳ���������ϵͳ����Ȼ�������������������������°�װ�ͻ��ˡ�");
		else if (dwErrorCode==RPC_S_INVALID_BOUND)
			ErrorInfo.strErrorInfo+=_T("��̨�������汾��ƥ�䣬����������ϵͳ���������ϵͳ����Ȼ�������������������������°�װ�ͻ��ˡ�");
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
					CString strErrorInfo;
					strErrorInfo.Format(_T("δ֪ϵͳ����0x%08X."),dwErrorCode);
					ErrorInfo.strErrorInfo+=strErrorInfo;
					return dwErrorCode;
				}

				ErrorInfo.strErrorInfo+=(LPCTSTR)lpMsgBuf;
				LocalFree(lpMsgBuf);
			}
			else 
			{
				TCHAR szExtendedErrorInfo[512];
				DWORD dwExtendedErrorCode, dwInfoBufferLen = _countof(szExtendedErrorInfo);
				InternetGetLastResponseInfo(&dwExtendedErrorCode, szExtendedErrorInfo, &dwInfoBufferLen);
				ErrorInfo.strErrorInfo += szExtendedErrorInfo;
				ErrorInfo.strErrorInfo.AppendFormat(_T("(����� %u)"), dwExtendedErrorCode);
			}

			ErrorInfo.strErrorInfo.TrimRight(_T("\r\n"));
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
	int nErrorInfoLen=ErrorInfo.strErrorInfo.GetLength()-1;
	LPCTSTR lpstrErrorInfo=ErrorInfo.strErrorInfo;
	for (int i=0;i<nErrorInfoLen;i++)
	{
		if (lpstrErrorInfo[i]!=':')
			continue;

		if (lpstrErrorInfo[i+1]=='/' || lpstrErrorInfo[i+1]=='\\' || (lpstrErrorInfo[i+1]>='0' && lpstrErrorInfo[i+1]<='9'))
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
		ErrorInfo.strErrorInfo = szTitle;
	else
		ErrorInfo.strErrorInfo=szTitle+CString((LPCTSTR)ErrorInfo.strErrorInfo+nIndex);

	return ErrorInfo.dwErrorCode;
}

void ResetErrorInfo()
{
	ERROR_INFO& ErrorInfo=GetThreadErrorInfo();
	ErrorInfo.dwErrorCode = ErrorInfo.dwErrorType =0;
	ErrorInfo.strErrorInfo.Empty();
}

void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...)
{
	ERROR_INFO& ErrorInfo=GetThreadErrorInfo();

	CString strErrorInfo = ErrorInfo.strErrorInfo;

	va_list paralist;
	va_start(paralist, lpszErrorTitle); 
	SetErrorInfoV(dwErrorType,dwErrorCode,lpszErrorTitle,paralist);

	if (strErrorInfo.IsEmpty() == FALSE)
		ErrorInfo.strErrorInfo = strErrorInfo + _T("\r\n") + ErrorInfo.strErrorInfo;
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