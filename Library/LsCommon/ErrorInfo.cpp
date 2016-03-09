#include "stdafx.h"
#include "ErrorInfo.h"
#include <shlwapi.h>
#include <WinInet.h>
#include "FileTools.h"
#include "NtDll.h"
#include <Winsock2.h>

/*
����˵��:���ڴ˴���Ϊ�˽����Ϊ�ⲿ�������乹�캯������֮ǰ��CPP��
��ĳЩȫ�ֱ���δ��ʼ�������µ��÷Ƿ��Ĵ���,�������ⲿ����֮ǰ����
��ʼ����CPP�е�ȫ�ֱ���
*/
#ifdef LS_STATIC_LIB_CALL
#pragma warning(disable:4073)
#pragma init_seg(lib)
#endif

DWORD g_dwTlsIndex=TlsAlloc();
extern CRITICAL_SECTION g_csWriteFileMutex;
HANDLE g_hErrorLogFile=INVALID_HANDLE_VALUE;

DWORD g_dwLogFileCodeType=0;//ANSI���뷽ʽ
DWORD g_dwLastError = ERROR_LG_SUCCESS;
LPCTSTR lpszErrorInfo[] = {
	{_T("�ɹ�")}
};

void ReleaseThreadErrorInfo()
{
	PERROR_INFO pErrorInfo=reinterpret_cast<PERROR_INFO>(TlsGetValue(g_dwTlsIndex));
	if (pErrorInfo) 
		delete pErrorInfo; 
	TlsSetValue(g_dwTlsIndex, NULL);
}
/*
void StartupProcessErrorInfo(LPCTSTR lpszLogFileName)
{
	InitializeCriticalSection(&g_csWriteFileMutex);
	if (lpszLogFileName)
	{
		g_strLogFileName=lpszLogFileName;
	}
}
*/

void ReleaseProcessErrorInfo()
{
	ReleaseThreadErrorInfo();
	DeleteCriticalSection(&g_csWriteFileMutex);
	TlsFree(g_dwTlsIndex);
	if (g_hErrorLogFile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hErrorLogFile);
		g_hErrorLogFile=INVALID_HANDLE_VALUE;
	}
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

BOOL WriteErrorInfoToLogFile(LPCTSTR lpszLogInfo,...)
{
	BOOL bSuccess=TRUE;

	try
	{
		EnterCriticalSection(&g_csWriteFileMutex);

		SYSTEMTIME SystemTime;
		::GetLocalTime(&SystemTime);

		static SYSTEMTIME stLogFileCreateDate={0};//��־�ļ�����ʱ��
		if (g_hErrorLogFile!=INVALID_HANDLE_VALUE)
		{
			if (stLogFileCreateDate.wYear!=SystemTime.wYear || stLogFileCreateDate.wMonth!=SystemTime.wMonth
				|| stLogFileCreateDate.wDay!=SystemTime.wDay)//�ж���־�ļ�����ʱ��͵�ǰʱ���Ƿ�Ϊ��ͬһ�죬�������´�������־�ļ�
			{
				CloseHandle(g_hErrorLogFile);
				g_hErrorLogFile=INVALID_HANDLE_VALUE;
			}
		}

		if (g_hErrorLogFile==INVALID_HANDLE_VALUE)
		{
			CString strLogFileName=GetLogFileName();
			g_hErrorLogFile=CreateFile(strLogFileName,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,0,OPEN_ALWAYS,0,0);
			if (g_hErrorLogFile==INVALID_HANDLE_VALUE)
			{
				static BOOL bFirstOpenError=TRUE;//ֻ��ʾһ�δ򿪴�����ʾ(�����û���ͬʱ�򿪽����´���־����)
				if (bFirstOpenError)
				{
					bFirstOpenError=FALSE;
					throw SetErrorInfo(SYSTEM_ERROR,0,_T("����־�ļ� %s ʧ��:"),strLogFileName);
				}
				else
				{
					LeaveCriticalSection(&g_csWriteFileMutex);
					return TRUE;
				}
			}

			SetFilePointer(g_hErrorLogFile,0,0,FILE_END);

			memcpy(&stLogFileCreateDate,&SystemTime,sizeof(SYSTEMTIME));
		}

		CString strLogInfo;

		if (lpszLogInfo)
		{
			va_list paralist;
			va_start(paralist, lpszLogInfo); 
			strLogInfo.FormatV(lpszLogInfo,paralist);
		}
		else
			strLogInfo=GetThreadErrorInfoString();

		CString szWriteInfo;
		szWriteInfo.Format(_T("%02d-%02d-%02d %02d:%02d:%02d   %s"),SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,
			SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond,strLogInfo);
		if (szWriteInfo.Right(2).Compare(_T("\r\n")))//ϵͳ����ʱ��������Ѿ����л��з�
			szWriteInfo+=_T("\r\n");

		DWORD dwNumberOfBytesWritten;
		if (WriteFile(g_hErrorLogFile,szWriteInfo,szWriteInfo.GetLength()*sizeof(TCHAR),&dwNumberOfBytesWritten,0)==FALSE)
			throw SetErrorInfo(SYSTEM_ERROR,0,_T("д��־�ļ���Ϣ %s ʧ��:"),szWriteInfo);
	}
	catch(...)
	{
		bSuccess=FALSE;
		::OutputDebugString(GetThreadErrorInfoString());
	}

	LeaveCriticalSection(&g_csWriteFileMutex);

	return bSuccess;
}

CString GetLogFileName()
{
	return GetLogFilePath(_T("Log"),_T(".log"));
}

CString GetLogFilePath(LPCTSTR lpszDirName,LPCTSTR lpszExtendName,LPCTSTR lpszVersion /* = NULL*/)
{
	ASSERT(lpszDirName && lstrlen(lpszDirName));
	ASSERT(lpszExtendName && lstrlen(lpszExtendName));

	TCHAR szLogPath[MAX_PATH];
	GetCommonAppDataFilePath(lpszDirName,_T(""),szLogPath);
	if (PathIsDirectory(szLogPath)==FALSE)
	{
		if (CreateDirectory(szLogPath,0)==FALSE)
			::PathRemoveFileSpec(szLogPath);
	}

	CString strExtension(lpszExtendName);
	strExtension.TrimLeft(_T("."));

	CString strVersion(lpszVersion);
	if (strVersion.IsEmpty() == FALSE)
		strVersion.Insert(0,_T("-"));

	SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);
	CString szFileName;
	szFileName.Format(_T("%s(%02d-%02d-%02d)%s.%s"),GetLogModuleFileName(),SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,strVersion,strExtension);
	if (szFileName.GetLength()+_tcslen(szLogPath)+2>MAX_PATH)
		::GetSystemDirectory(szLogPath,MAX_PATH);

	::PathAppend(szLogPath,szFileName);
	return szLogPath;
}

BOOL SetLogFileCodeType(DWORD dwCodeType)
{
	if (g_dwLogFileCodeType==dwCodeType)
		return TRUE;
	else if (g_hErrorLogFile==INVALID_HANDLE_VALUE)//��־�ļ���δ��
	{
		g_dwLogFileCodeType=dwCodeType;
		return TRUE;
	}
	SetErrorInfo(CUSTOM_ERROR,0,_T("������־�ļ����뷽ʽʧ��:��־�ļ��Ѵ�."));
	return FALSE;
}

void LanGuardSetLastError(DWORD dwError)
{
	if (dwError & 0x80000000)
	{
		dwError = ERROR_LG_INVALID_PARAMETER - dwError;
	}

	if (g_dwLastError == ERROR_LG_SUCCESS || dwError == ERROR_LG_SUCCESS)
	{
		g_dwLastError = dwError;
	}
}

DWORD LanGuardGetLastError()
{
	return g_dwLastError;
}

LPCTSTR LanGuardGetLastErrorString()
{
	return lpszErrorInfo[g_dwLastError];
}

CString GetLogModuleFileName()
{
	static CString s_strLogFileName;
	if (s_strLogFileName.IsEmpty())
	{
		InitializeCriticalSection(&g_csWriteFileMutex);

		TCHAR szExeTitle[MAX_PATH];
		::GetModuleFileName(0,szExeTitle,MAX_PATH);
		PathRemoveExtension(szExeTitle);
		s_strLogFileName = PathFindFileName(szExeTitle);
	}
	return s_strLogFileName;
}