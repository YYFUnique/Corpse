#include "Stdafx.h"
#include "TextTools.h"
#include "ErrorInfo.h"
#include "Class/FsRedirectHelper.h"
#include <math.h>

#define    PIPE_BUFFER_LEN		1024*10		//定义管道缓冲区长度

#pragma comment(lib,"Rpcrt4.lib")
CString UTF8ToUniCode(LPCSTR lpszUTF8)
{
	int nLen = MultiByteToWideChar(CP_UTF8,0,lpszUTF8,-1,0,0);

	wchar_t* pUnicode = new wchar_t[nLen+1];
	MultiByteToWideChar(CP_UTF8,0,lpszUTF8,-1,pUnicode,nLen);

	pUnicode[nLen] = L'\0';
	CString strUnicode(pUnicode);

	delete[] pUnicode;

	return strUnicode;
}

CStringA UnicodeToUTF8(LPWSTR lpszUnicode)
{
	int nLen = WideCharToMultiByte(CP_UTF8,0,lpszUnicode,-1,NULL,0,NULL,NULL);

	char* pUTF8 = new char[nLen+1];
	WideCharToMultiByte(CP_UTF8,0,lpszUnicode,-1,pUTF8,nLen,NULL,NULL);
	pUTF8[nLen]='0';
	
	CStringA strUTF8(pUTF8);
	delete[] pUTF8;

	return strUTF8;
}

CStringA UnicodeToAnsi(LPWSTR lpszUnicode)
{
	int nLen = WideCharToMultiByte(CP_ACP,0,lpszUnicode,-1,NULL,0,NULL,NULL);

	char* pszAnsi = new char[nLen+1];
	if (pszAnsi == NULL)
		return NULL;

	WideCharToMultiByte(CP_ACP,0,lpszUnicode,-1,pszAnsi,nLen,NULL,NULL);
	pszAnsi[nLen] = '\0';
	
	CStringA strAnsi(pszAnsi);
	delete[] pszAnsi;
	return strAnsi;
}

void AnalyseDbNullTerminatedText(LPCTSTR lpszFormattedInfo,CStringArray& InfoArray)
{
	InfoArray.RemoveAll();
	if (lpszFormattedInfo==NULL)
		return;

	while(*lpszFormattedInfo != '\0')
	{
		InfoArray.Add(lpszFormattedInfo);
		lpszFormattedInfo+=_tcslen(lpszFormattedInfo)+1;
	}
}

CString ConvertBinToString(const void* pBuffer,DWORD dwBufferLen)
{
	CString strReturnText;
	
	for(DWORD i=0;i<dwBufferLen;i++)
	{
		CString strText;
		strText.Format(_T("02X"),*((BYTE*)pBuffer+i));
		strReturnText += strText;
	}

	return strReturnText;
}

CString FormatNumSizeToStringSize(ULONGLONG ulFileSizeInByte)
{
	CString strFileSize;
	if (ulFileSizeInByte < 1024)
		strFileSize.Format(_T("%lu B"),ulFileSizeInByte);
	if (ulFileSizeInByte<(1024*1024))
		strFileSize.Format(_T("%lu KB"),(DWORD)ceil(ulFileSizeInByte/1024.0));
	else if (ulFileSizeInByte<1024*1024*1024)	
		strFileSize.Format(_T("%lu MB"),(DWORD)ceil((ulFileSizeInByte/1024.0)/1024));
	else 
		strFileSize.Format(_T("%lu GB"),(DWORD)ceil((ulFileSizeInByte/1024.0/1024)/1024));

	return strFileSize;
}

ULONGLONG ConvertStringSizeToNum(LPCTSTR lpszStringSize)
{
	ULONGLONG ulNum;
	ULONGLONG ulMulti=1;
	CString strStringSize(lpszStringSize);

	strStringSize.TrimRight(_T("B"));

	if (strStringSize.Find(_T("K")) != -1)
	{
		strStringSize.TrimRight(_T("K"));
		ulMulti = 1024;
	}
	else if(strStringSize.Find(_T("M")) != -1)
	{
		strStringSize.TrimRight(_T("M"));
		ulMulti = 1024*1024;
	}
	else if (strStringSize.Find(_T("G")) != -1)
	{
		strStringSize.TrimRight(_T("G"));
		ulMulti = 1024*1024*1024;
	}

	_stscanf_s(strStringSize,_T("%I64u"),&ulNum);

	return ulNum*ulMulti;
}

CString FormatNumToString(DWORD dwNum)
{
	CString strFormatString;
	strFormatString.Format(_T("%u"),dwNum);
	return strFormatString;
}

CString FormatNumToHexString(DWORD dwNum)
{
	CString strFormatString;
	strFormatString.Format(_T("0x%08X"),dwNum);
	return strFormatString;
}

CString FormatNumToHexStringNoPre(DWORD dwNum)
{
	CString strFormatString;
	strFormatString.Format(_T("%08X"),dwNum);
	return strFormatString;
}

CString FormatTimeToString(ULONGLONG ulTime)
{
	CString strAppointedTime;
	CTime TimeString(ulTime);
	strAppointedTime.Format(_T("%d-%02d-%02d %02d:%02d:%02d"),
				TimeString.GetYear(),TimeString.GetMonth(),TimeString.GetDay(),
				TimeString.GetHour(),TimeString.GetMinute(),TimeString.GetSecond());
	return strAppointedTime;
}

CString GetHandleDes(HANDLE dwObjType,OBJECT_INFORMATION_CLASS dwQueryType)
{
	TCHAR szObjectName[MAX_PATH];
	DWORD dwSize = 0;
	CString strRetValue;

	NTSTATUS status = NtQueryObject(dwObjType,dwQueryType,szObjectName,sizeof(szObjectName),&dwSize);
	if (NT_SUCCESS(status))
	{
		PUNICODE_STRING strHandle = (PUNICODE_STRING)szObjectName;
		CString strHandleDes(strHandle->Buffer,strHandle->Length);
		strRetValue = strHandleDes;
	}

	return strRetValue;
}

CString GetGuidString(GUID DevGuid)
{
	CString strGuid;
#ifndef _UNICODE
	unsigned char* szGuid;
#else
	unsigned short* szGuid;
#endif
	if(RPC_S_OK==UuidToString(&DevGuid,&szGuid))
	{
		strGuid.Format(_T("{%s}"),(LPTSTR)szGuid);
	}

	return strGuid;
}


DWORD GetDoubleNullWideStringLen(LPCWSTR lpszWideString)
{
	DWORD dwSize = 0;
	LPCWSTR lpszWideChar = lpszWideString;
	while(*lpszWideChar)
	{
		dwSize += (wcslen(lpszWideChar) + 1) * sizeof(WCHAR);
		lpszWideChar += wcslen(lpszWideChar) + 1;
	}

	dwSize += 1;
	return dwSize;
}

BOOL ConvertDoubleNullWideStringToStringArray(LPCWSTR lpszWideString,CStringArray& strInfoArray)
{
	BOOL bSuccess = TRUE;
	LPCWSTR lpszMultiByte = lpszWideString;
	int nGBLen = 0;

	while(*lpszMultiByte)
	{
		CString strMultiString(lpszMultiByte);
		strInfoArray.Add(strMultiString);

		lpszMultiByte += wcslen(lpszMultiByte) + 1;
	}

	return bSuccess;
}

BOOL ConvertStringArrayToDoubleNullWideString(CStringArray& strInfoArray,LPWSTR lpszWideString,DWORD& dwSize)
{
	BOOL bSuccess = FALSE;
	LPWSTR lpszWideTmp = lpszWideString;
	int i=0;
	DWORD dwWideLen = 0;

	for (i=0;i<strInfoArray.GetSize();++i)
	{
		const CString& strMultiByteString = strInfoArray.GetAt(i);

		CStringW strWideChar(strMultiByteString);

		wcscpy_s(lpszWideTmp,strWideChar.GetLength()*sizeof(TCHAR),strWideChar);

		//当前宽字节字符串长度，包含结束符
		dwWideLen += (strWideChar.GetLength() + 1) * sizeof(WCHAR);
		//将字符串末尾一个符号置为结束符
		lpszWideTmp[strWideChar.GetLength()] = L'\0';

		lpszWideTmp += strWideChar.GetLength() +1;
		//增加一个结束符
	}

	if (i == strInfoArray.GetSize() &&i != 0)
		bSuccess = TRUE;

	//末尾增加一个结束符
	dwWideLen += 1;
	lpszWideString[dwWideLen] = L'\0';

	return bSuccess;
}

CString GetCurrentFormatTime(CURRENT_FORMAT_TIME CurrentFormatTime)
{
	CString strCurrentFormatTime;
	time_t tmNow;
	CTime Time(time(&tmNow));
	
	switch(CurrentFormatTime)
	{
		case CURRENT_FORMAT_TIME_LongTime:
				
			break;
		default:
			strCurrentFormatTime.Format(_T("%d-%02d-%02d %02d:%02d:%02d"),
							Time.GetYear(),Time.GetMonth(),Time.GetDay(),
							Time.GetHour(),Time.GetMinute(),Time.GetSecond());
			break;
	}
	
	return strCurrentFormatTime;
}

BOOL CopyDataToClipboard(DWORD dwDataType,HWND hWndNewOwner,LPCTSTR lpszText)
{
	BOOL bCloseClipboard = FALSE;
	HGLOBAL hMem = NULL;
	BOOL bSuccess = FALSE;

	do
	{
		if (::OpenClipboard(hWndNewOwner)==0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开剪贴板失败"));
			break;
		}

		bCloseClipboard = TRUE;

		if (EmptyClipboard() == 0)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("清空剪贴板内容失败"));
			break;
		}

		if (dwDataType == CF_UNICODETEXT)
		{
			hMem=GlobalAlloc(GMEM_MOVEABLE,(lstrlen(lpszText)+1)*sizeof(TCHAR));
			if (hMem == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("设置剪贴板文本内容时分配内存失败"));
				break;
			}

			lstrcpy((TCHAR*)GlobalLock(hMem),lpszText);
			GlobalUnlock(hMem);
		}
		else if (dwDataType == CF_HDROP)
		{
			DROPFILES DropFile;
			DropFile.pFiles = sizeof(DROPFILES);
			DropFile.pt.x = 0;DropFile.pt.y = 0;
			DropFile.fNC = FALSE;
#ifndef UNICODE
			DropFile.fWide = FALSE;
#else
			DropFile.fWide = TRUE;
#endif
			DWORD dwSize = sizeof(DROPFILES) + _tcslen(lpszText)*sizeof(TCHAR) + 2;
			hMem=GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,dwSize);
			LPBYTE lpData = (LPBYTE)GlobalLock(hMem);
			memcpy(lpData,&DropFile,sizeof(DROPFILES));	
			memcpy(lpData+sizeof(DROPFILES),lpszText,_tcslen(lpszText)*sizeof(TCHAR));
			GlobalUnlock(hMem);	//普通的剪贴板操作
		}

		bSuccess = SetClipboardData(dwDataType,hMem) != NULL;
		if (bSuccess)
			hMem = NULL;//成功后不需要释放内存
		else
			SetErrorInfo(SYSTEM_ERROR,0,_T("设置文本内容到剪贴板失败"));
	}while(0);

	if (bCloseClipboard)
		CloseClipboard();

	if (hMem)
		GlobalFree(hMem);

	return bSuccess;
}


BOOL PrintNumberWithSeparator(ULONGLONG ullNumber,TCHAR szSeparator ,CString& strNumWithSeparator)
{
	const int nSeparatorNum = 3;
	BOOL bSuccess = FALSE;
	do 
	{
		strNumWithSeparator.Empty();
		CString strNumber;
		strNumber.Format(_T("I64"),ullNumber);
		UINT nTotal = strNumber.GetLength()*sizeof(TCHAR); 
		UINT nMod = nTotal/nSeparatorNum;
		if (nTotal>nSeparatorNum)
			strNumWithSeparator += strNumber.Left(nMod);

		UINT nStart = nMod;
		while(nStart < nTotal)
		{
			strNumWithSeparator += _T(",");
			strNumWithSeparator += strNumber.Mid(nStart,nSeparatorNum);
			nTotal += nSeparatorNum;
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}


BOOL RedirectionOutput(LPCTSTR lpszExeFilePath,LPCTSTR lpszParam,DWORD dwWaitTime,CString& strOutput)
{
	BOOL bSuccess = FALSE;
	HANDLE hRead = INVALID_HANDLE_VALUE,hWrite = INVALID_HANDLE_VALUE;
	STARTUPINFO si; 
	PROCESS_INFORMATION pi;
	do 
	{
		ZeroMemory(&pi, sizeof(pi));
		ZeroMemory(&si,sizeof(STARTUPINFO));

		SECURITY_ATTRIBUTES sa; 
		sa.nLength=sizeof(SECURITY_ATTRIBUTES); 
		sa.lpSecurityDescriptor= NULL;			//使用系统默认的安全描述符
		sa.bInheritHandle= TRUE;					//创建的进程继承句柄

		if (CreatePipe(&hRead,&hWrite,&sa,0) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("创建匿名管道失败"));
			break;
		}

		si.cb=sizeof(STARTUPINFO); 
		GetStartupInfo(&si); 
		si.hStdError= hWrite; 
		si.hStdOutput= hWrite;//新创建进程的标准输出连在写管道一端
		si.wShowWindow= SW_HIDE;//隐藏窗口
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

		//关闭文件重定向
		CFsRedirectHelper Helper(TRUE);

		if (CreateProcess(lpszExeFilePath,(LPTSTR)lpszParam,&sa,NULL,TRUE,NULL,NULL,NULL,&si,&pi) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("执行程序[%s %s]失败"),lpszExeFilePath,lpszParam);
			break;
		}

		DWORD dwRet = WaitForSingleObject(pi.hProcess,dwWaitTime);
		if (dwRet == WAIT_TIMEOUT)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("执行程序，获取信息等待时间超时"));
			break;
		}

		TCHAR szData[PIPE_BUFFER_LEN];
		DWORD dwSize = 0;

		if (ReadFile(hRead,szData,PIPE_BUFFER_LEN,&dwSize,NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("从管道中读取输出内容失败"));
			break;
		}

		strOutput = CString(szData,dwSize);

		bSuccess = TRUE;
	} while (FALSE);

	if (pi.hThread != NULL)
		CloseHandle(pi.hThread);
	if (pi.hProcess != NULL)
		CloseHandle(pi.hProcess);

	if (hRead != INVALID_HANDLE_VALUE)
		CloseHandle(hRead);
	if (hWrite != INVALID_HANDLE_VALUE)
		CloseHandle(hWrite);

	return bSuccess;
}

BOOL GetUserPicturePath(LPCTSTR lpszUserName,CString& strPicturePath)
{
	HINSTANCE hInst = LoadLibrary(_T("shell32.dll"));   
	if( hInst )   
	{   
		typedef LRESULT(WINAPI *GETUSREPICTUREPATH)(LPCWSTR,DWORD,LPWSTR,UINT);
		GETUSREPICTUREPATH GetUserPictruePath;   
		GetUserPictruePath = (GETUSREPICTUREPATH)GetProcAddress(hInst,(char*)233);   

		TCHAR szUserName[MAX_PATH],szFilePath[MAX_PATH];
		DWORD dwSize = _countof(szUserName);
		GetUserName(szUserName,&dwSize);
		if( GetUserPictruePath )
		{
			GetUserPictruePath(szUserName,0,szFilePath,_countof(szFilePath));
			/*strPicturePath*/
		}

		FreeLibrary( hInst );   
	} 
	return TRUE;
}

void AnalyseFormattedTextInfoItem(const TCHAR* lpszFormattedInfo,CStringArray& InfoArray,TCHAR ch)
{
	InfoArray.RemoveAll();

	while(1)
	{
		const TCHAR* pSeparator=_tcschr(lpszFormattedInfo,ch);
		if (pSeparator)
			InfoArray.Add(CString(lpszFormattedInfo,(int)(pSeparator-lpszFormattedInfo)));
		else
		{
			pSeparator=lpszFormattedInfo+_tcslen(lpszFormattedInfo);
			if (pSeparator!=lpszFormattedInfo)
				InfoArray.Add(CString(lpszFormattedInfo,(int)(pSeparator-lpszFormattedInfo)));
			break;
		}
		lpszFormattedInfo=pSeparator+1;
	}
}