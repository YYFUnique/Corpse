#include "stdafx.h"
#include "LogHelper.h"
#include "../Utils/FileTools.h"
#include "../Sync/SyncLock.h"
#include <atltime.h>

QLogImpl* QLogImpl::GetInstance()
{
	//不是多线程安全类型
	static QLogImpl sLogImpl;
	return &sLogImpl;
}

QLogImpl::QLogImpl()
{
	SetLogLevel(LOG_LEVEL_ALL);
}

void QLogImpl::SetLogLevel(LOG_LEVEL LogLevel)
{
	if (LogLevel > LOG_LEVEL_ALL || LogLevel < LOG_LEVEL_ERR)
		return;
	m_LogLevel = LogLevel;
}

void QLogImpl::WriteLog(LOG_LEVEL LogLevel, LPCTSTR lpszLogInfo)
{
	HANDLE hLogFile = INVALID_HANDLE_VALUE;

	do 
	{
		if (LogLevel > m_LogLevel)
			break;

		CString strLogFilePath = GetProgramDataFilePath(_T("Log"), _T("log"), GetModuleFileVersion());
		hLogFile = CreateFile(strLogFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
		if (hLogFile == INVALID_HANDLE_VALUE)
			break;

		DWORD dwMovePointer = SetFilePointer(hLogFile, 0, 0, FILE_END);

#if UNICODE
		//返回值为空，表示移动文件指针距离为0，也就是打开空文件，需要给文件头添加0xFF,0xFE
		if (dwMovePointer == NULL)
		{
			CHAR szUTF8Head[] = {(char)0xFF,(char)0xFE};
			DWORD dwDataLen = _countof(szUTF8Head);
			WriteFile(hLogFile, szUTF8Head, dwDataLen, &dwDataLen, 0);
		}
#endif // !UNICODE

		DWORD dwNumberOfBytesWritten;
		CString strDebugLogInfo(lpszLogInfo);
		WriteFile(hLogFile, strDebugLogInfo, strDebugLogInfo.GetLength()*sizeof(TCHAR), &dwNumberOfBytesWritten, 0);

	} while (FALSE);

	if (hLogFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hLogFile);
		hLogFile = INVALID_HANDLE_VALUE;
	}
}

QLogHelper::QLogHelper(LPCSTR lpszFileName, LONG lFileLine)
{
	m_bLogToFile = TRUE;
	m_strFileName = lpszFileName;
	m_nFileLine = lFileLine;
}

QLogHelper::~QLogHelper()
{
	if (m_bLogToFile == FALSE)
		return;

	LPCTSTR lpszLogLevel = _T("INFO");
	switch (m_LogLevel)
	{
	case LOG_LEVEL_ERR:
			lpszLogLevel = _T("ERROR");
		break;
	case LOG_LEVEL_WAR:
			lpszLogLevel = _T("WARN");
		break;
	case LOG_LEVEL_APP:
			lpszLogLevel = _T("APP");
		break;
	case LOG_LEVEL_PRO:
			lpszLogLevel = _T("PROJECT");
		break;
	case LOG_LEVEL_INFO:
			lpszLogLevel = _T("INFO");
		break;
	default:
			ASSERT(FALSE);
		break;
	}

	CString strTipMsg;
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	strTipMsg.Format(_T("[%s][%5d][%02d:%02d:%02d:%03d]:%s\r\n"),
								lpszLogLevel,GetCurrentProcessId(),SysTime.wHour,SysTime.wMinute,
								SysTime.wSecond,SysTime.wMilliseconds, m_strLogInfo);
	
	QLogImpl::GetInstance()->WriteLog(m_LogLevel, strTipMsg);
}

void QLogHelper::VLog(LOG_LEVEL LogLevel, LPCTSTR lpszFormat,...)
{
	va_list paralist;
	va_start(paralist, lpszFormat); 
	m_LogLevel = LogLevel;
	m_strLogInfo.FormatV(lpszFormat,paralist);
	m_strLogInfo.TrimRight(_T("\r\n"));
}

void QLogHelper::VLog(LPCTSTR lpszFormat,...)
{
	m_bLogToFile = FALSE;
	va_list paralist;
	va_start(paralist, lpszFormat); 

	CString strLogInfo;
	strLogInfo.FormatV(lpszFormat, paralist);
	strLogInfo.TrimRight(_T("\r\n"));
	OutputDebugString(strLogInfo);
}