#include "stdafx.h"
#include "LsErrorInfo.h"
#include <shlwapi.h>
#include <Wininet.h>
//#include "LsFileTools.h"

DWORD g_dwTlsIndex=TlsAlloc();

#ifdef _DEBUG
LS_DEBUG_MODE g_DebugMode = LS_DEBUG_MODE_NORMAL;
#else
LS_DEBUG_MODE g_DebugMode = LS_DEBUG_MODE_NONE;
#endif


HANDLE g_hErrorLogFile = INVALID_HANDLE_VALUE;

void ReleaseThreadErrorInfo()
{
	PLS_ERROR_INFO pLsErrorInfo=reinterpret_cast<PLS_ERROR_INFO>(TlsGetValue(g_dwTlsIndex));
	if (pLsErrorInfo) 
		delete pLsErrorInfo; 
	TlsSetValue(g_dwTlsIndex, NULL);
}

void StartupProcessErrorInfo()
{
}

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

LS_ERROR_INFO& GetThreadErrorInfo()
{
	DWORD dwLastErrorCode = GetLastError();

	PLS_ERROR_INFO pLsErrorInfo=reinterpret_cast<PLS_ERROR_INFO>(TlsGetValue(g_dwTlsIndex));
	if (pLsErrorInfo == NULL) 
	{ 
		pLsErrorInfo = new LS_ERROR_INFO;
		if (pLsErrorInfo != NULL) 
			TlsSetValue(g_dwTlsIndex, pLsErrorInfo);
	}
	SetLastError(dwLastErrorCode);

	return *pLsErrorInfo;
}

CString& GetThreadErrorInfoString()
{
	LS_ERROR_INFO& LsErrorInfo=GetThreadErrorInfo();

	return LsErrorInfo.szErrorInfo;
}

BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode)
{
	LS_ERROR_INFO& LsErrorInfo=GetThreadErrorInfo();
	if (LsErrorInfo.dwErrorType != dwErrorType)
		return FALSE;
	else if (dwErrorType == SYSTEM_ERROR)
		return LsErrorInfo.dwErrorCode == dwErrorCode;
	else if (dwErrorType == CUSTOM_ERROR)
		if (dwErrorCode == 0xffffffff)
			return TRUE;
		else
			return LsErrorInfo.dwErrorCode == dwErrorCode;

	return FALSE;
}

DWORD SetErrorInfoV(LS_ERROR_INFO& LsErrorInfo , DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList)
{
	/*if (dwErrorCode==0)
	{
		dwErrorCode=0xffffffff;
		if (dwErrorType==SYSTEM_ERROR)
			dwErrorCode = GetLastError();
		else if (dwErrorType==WINSOCK_ERROR)
		{
			dwErrorCode = WSAGetLastError();
			dwErrorType = SYSTEM_ERROR;
		}
	}

	LsErrorInfo.dwErrorCode=dwErrorCode;
	LsErrorInfo.dwErrorType=dwErrorType;

	if (lpszErrorTitle)
		LsErrorInfo.szErrorInfo.FormatV(lpszErrorTitle,argList);
	else
		LsErrorInfo.szErrorInfo.Empty();

	if (dwErrorType==SYSTEM_ERROR)
	{
		//自动增加冒号
		if (!LsErrorInfo.szErrorInfo.IsEmpty())
			if (LsErrorInfo.szErrorInfo.Right(1).Compare(_T(":")))
				LsErrorInfo.szErrorInfo+=_T(":");

		HMODULE hMod = NULL;
		if (LsErrorInfo.dwErrorCode>=12000 && LsErrorInfo.dwErrorCode<=12174)
			hMod = GetModuleHandle(_T("wininet.dll"));

		if (dwErrorCode != ERROR_INTERNET_EXTENDED_ERROR)
		{
			DWORD dwFlag = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
			if (hMod)
				dwFlag |= FORMAT_MESSAGE_FROM_HMODULE;

			LPVOID lpMsgBuf;
			if (!FormatMessage(dwFlag,hMod,dwErrorCode,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0,NULL))
			{
				CString szErrorInfo;
				szErrorInfo.Format(_T("未知系统错误0x%08X."),dwErrorCode);
				LsErrorInfo.szErrorInfo+=szErrorInfo;
				return dwErrorCode;
			}

			LsErrorInfo.szErrorInfo+=(LPCTSTR)lpMsgBuf;
			LocalFree(lpMsgBuf);
		}
		else
		{
			TCHAR szExtendedErrorInfo[512];
			DWORD dwExtendedErrorCode , dwInfoBufferLen = _countof(szExtendedErrorInfo);
			InternetGetLastResponseInfo(&dwExtendedErrorCode , szExtendedErrorInfo , &dwInfoBufferLen);
			LsErrorInfo.szErrorInfo += szExtendedErrorInfo;
			LsErrorInfo.szErrorInfo.AppendFormat(_T("(错误号 %u)") , dwExtendedErrorCode);
		}
		LsErrorInfo.szErrorInfo.TrimRight(_T("\r\n"));
	}

	return dwErrorCode;
	*/
	return TRUE;
}

DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...)
{
	va_list paralist;
	va_start(paralist, lpszErrorTitle); 

	LS_ERROR_INFO& LsErrorInfo=GetThreadErrorInfo();
	return SetErrorInfoV(LsErrorInfo,dwErrorType,dwErrorCode,lpszErrorTitle,paralist);
}

DWORD SetErrorTitle(LPCTSTR lpszTitle,...)
{
	LS_ERROR_INFO& LsErrorInfo=GetThreadErrorInfo();
	int nIndex=-1;
	int nErrorInfoLen=LsErrorInfo.szErrorInfo.GetLength()-1;
	LPCTSTR lpszErrorInfo=LsErrorInfo.szErrorInfo;
	for (int i=0;i<nErrorInfoLen;i++)
	{
		if (lpszErrorInfo[i]!=':')
			continue;

		if (lpszErrorInfo[i+1]=='/' || lpszErrorInfo[i+1]=='\\' || (lpszErrorInfo[i+1]>='0' && lpszErrorInfo[i+1]<='9'))
			continue;

		nIndex=i;
		break;
	}

	if (nIndex==-1)
		LsErrorInfo.szErrorInfo=lpszTitle;
	else
	{
		CString szTitle;
		if (lpszTitle)
		{
			va_list paralist;
			va_start(paralist, lpszTitle); 
			szTitle.FormatV(lpszTitle,paralist);
		}
		szTitle.TrimRight(':');
		LsErrorInfo.szErrorInfo=szTitle+CString((LPCTSTR)LsErrorInfo.szErrorInfo+nIndex);
	}
	return LsErrorInfo.dwErrorCode;
}

void ResetErrorInfo()
{
	LS_ERROR_INFO& LsErrorInfo=GetThreadErrorInfo();
	LsErrorInfo.dwErrorCode = LsErrorInfo.dwErrorType =0;
	LsErrorInfo.szErrorInfo.Empty();
}

void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...)
{
	LS_ERROR_INFO& LsErrorInfo=GetThreadErrorInfo();

	CString strErrorInfo = LsErrorInfo.szErrorInfo;

	va_list paralist;
	va_start(paralist, lpszErrorTitle); 
	SetErrorInfoV(LsErrorInfo,dwErrorType,dwErrorCode,lpszErrorTitle,paralist);

	if (strErrorInfo.IsEmpty() == FALSE)
		LsErrorInfo.szErrorInfo = strErrorInfo + _T("\r\n") + LsErrorInfo.szErrorInfo;
}

//设置调试模式
void SetDebugMode(LS_DEBUG_MODE dwDebugMode)
{
	g_DebugMode = dwDebugMode;
}

DWORD SetErrorInfoV2(LPCSTR lpszFileName,DWORD dwLine,LPCSTR lpszFunctionName,DWORD dwFlag,LS_ERROR_TYPE dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...)
{
	if (dwFlag == ERROR_FLAG_DEBUG_INFO && g_DebugMode == LS_DEBUG_MODE_NONE)
		return 0;

	LS_ERROR_INFO LsErrorInfo;

	va_list paralist;
	va_start(paralist, lpszErrorTitle); 
	SetErrorInfoV(LsErrorInfo,dwErrorType,dwErrorCode,lpszErrorTitle,paralist);

	if (dwFlag & ERROR_FLAG_ERROR_INFO)
		GetThreadErrorInfo() = LsErrorInfo;
	if (dwFlag & ERROR_FLAG_DEBUG_INFO && g_DebugMode != LS_DEBUG_MODE_NONE)
	{
		SYSTEMTIME CurrentTime;
		GetLocalTime(&CurrentTime);

		CString strOutputString;
		strOutputString.Format(_T("%04d-%02d-%02d %02d:%02d:%02d:%03d %hs(%hs-%u) %s\r\n"),CurrentTime.wYear,CurrentTime.wMonth,
			CurrentTime.wDay,CurrentTime.wHour,CurrentTime.wMinute,CurrentTime.wSecond,CurrentTime.wMilliseconds,
			PathFindFileNameA(lpszFileName),lpszFunctionName,dwLine,LsErrorInfo.szErrorInfo);
		OutputDebugString(strOutputString);
	}
	if (dwFlag & ERROR_FLAG_LOG_FILE_INFO)
		WriteErrorInfoToLogFile(LsErrorInfo.szErrorInfo);

	return LsErrorInfo.dwErrorCode;
}

CString GetDefaultLogFileName()
{

	TCHAR szLogPath[1024];

	//由于没有 LsFileTools.h 屏蔽该函数 
	//并且该函数 失效
	//GetCommonAppDataFilePath(_T("Log"),_T(""),szLogPath);
	return _T("");

	if (PathIsDirectory(szLogPath)==FALSE)
	{
		if (CreateDirectory(szLogPath,0)==FALSE)
			::PathRemoveFileSpec(szLogPath);
	}

	SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);

	TCHAR szMainAppName[MAX_PATH];
	GetModuleFileName(0,szMainAppName,_countof(szMainAppName));
	PathRemoveExtension(szMainAppName);

	CString strFileName;
	strFileName.Format(_T("%s(%02d-%02d-%02d).log"),PathFindFileName(szMainAppName),SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay);

	if (strFileName.GetLength()+_tcslen(szLogPath)+2>_countof(szLogPath))
		::GetSystemDirectory(szLogPath,_countof(szLogPath));

	::PathAppend(szLogPath,strFileName);
	return szLogPath;
}

HANDLE GetLogFileHandle()
{
	SYSTEMTIME CurrentTime;
	::GetLocalTime(&CurrentTime);

	static SYSTEMTIME stLogFileCreateDate;//日志文件创建时间
	if (g_hErrorLogFile != INVALID_HANDLE_VALUE)
	{
		if (stLogFileCreateDate.wYear != CurrentTime.wYear || stLogFileCreateDate.wMonth != CurrentTime.wMonth
			|| stLogFileCreateDate.wDay != CurrentTime.wDay)//判断日志文件创建时间和当前时间是否为向同一天，否则将重新创建新日志文件
		{
			CloseHandle(g_hErrorLogFile);
			g_hErrorLogFile=INVALID_HANDLE_VALUE;
		}
	}

	if (g_hErrorLogFile==INVALID_HANDLE_VALUE)
	{
		CString strLogFileName = GetDefaultLogFileName();
		g_hErrorLogFile=CreateFile(strLogFileName,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,0,OPEN_ALWAYS,0,0);
		if (g_hErrorLogFile==INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(ERROR_FLAG_DEBUG_INFO , SYSTEM_ERROR,0,_T("打开日志文件 %s 失败:"),strLogFileName);
			return INVALID_HANDLE_VALUE;
		}

		DWORD dwFileOffset = SetFilePointer(g_hErrorLogFile,0,0,FILE_END);
#ifdef UNICODE
		if (dwFileOffset == 0)
		{
			DWORD dwNumberOfBytesWritten;
			const static BYTE UnicodeFileHead[2]={0xff,0xfe};
			WriteFile(g_hErrorLogFile,UnicodeFileHead,sizeof(UnicodeFileHead),&dwNumberOfBytesWritten,0);
		}
#endif
		memcpy(&stLogFileCreateDate,&CurrentTime,sizeof(SYSTEMTIME));
	}

	return g_hErrorLogFile;
}

BOOL WriteErrorInfoToLogFile(LPCTSTR lpszLogInfo,...)
{
	BOOL bSuccess = FALSE;

	do
	{
		HANDLE hLogFile = GetLogFileHandle();
		if (hLogFile == INVALID_HANDLE_VALUE)
			break;

		CString strLogInfo;

		if (lpszLogInfo)
		{
			va_list paralist;
			va_start(paralist, lpszLogInfo); 
			strLogInfo.FormatV(lpszLogInfo,paralist);
		}
		else
			strLogInfo = GetThreadErrorInfoString();

		SYSTEMTIME CurrentTime;
		::GetLocalTime(&CurrentTime);

		CString strWriteInfo;
		strWriteInfo.Format(_T("%02d-%02d-%02d %02d:%02d:%02d   %s"),CurrentTime.wYear,CurrentTime.wMonth,CurrentTime.wDay,
			CurrentTime.wHour,CurrentTime.wMinute,CurrentTime.wSecond,strLogInfo);

		if (strWriteInfo.Right(2).Compare(_T("\r\n")))//系统错误时可能最后已经带有换行符
			strWriteInfo+=_T("\r\n");

		DWORD dwNumberOfBytesWritten;
		if (WriteFile(hLogFile,strWriteInfo,strWriteInfo.GetLength()*sizeof(TCHAR),&dwNumberOfBytesWritten,0)==FALSE)
			SetErrorInfo(ERROR_FLAG_DEBUG_INFO , SYSTEM_ERROR,0,_T("写日志文件信息 %s 失败:"),strWriteInfo);

		bSuccess = TRUE;
	}while(0);

	return bSuccess;
}

BOOL OutputStringToDebugerEx(LPCSTR lpszFileName , DWORD dwLine , LPCSTR lpszFunctionName , LPCTSTR lpszDebugInfo ,...)
{
	if (g_DebugMode == LS_DEBUG_MODE_NONE)
		return TRUE;

	CString strDebugInfo;

	if (lpszDebugInfo)
	{
		va_list paralist;
		va_start(paralist, lpszDebugInfo); 
		strDebugInfo.FormatV(lpszDebugInfo , paralist);
	}
	else
		strDebugInfo = GetThreadErrorInfoString();

	SYSTEMTIME CurrentTime;
	::GetLocalTime(&CurrentTime);

	CString strOutputString;
	strOutputString.Format(_T("%04d-%02d-%02d %02d:%02d:%02d:%03d %hs(%hs-%u) %s\r\n"),CurrentTime.wYear,CurrentTime.wMonth,
		CurrentTime.wDay,CurrentTime.wHour,CurrentTime.wMinute,CurrentTime.wSecond,CurrentTime.wMilliseconds,
		PathFindFileNameA(lpszFileName) , lpszFunctionName , dwLine , strDebugInfo);

	OutputDebugString(strOutputString);
	return TRUE;
}