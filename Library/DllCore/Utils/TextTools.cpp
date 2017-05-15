#include "StdAfx.h"
#include "TextTools.h"
#include "StdPtr.h"

#define GB2312_CODE_PAGE 936

LPSTR Gb32ToUtf8(LPCSTR lpszGb32Text)
{
	int nUnicodeBufLen=MultiByteToWideChar(GB2312_CODE_PAGE , 0, lpszGb32Text, -1, 0, 0);
	if (nUnicodeBufLen==0)
		return NULL;

	WCHAR* pUnicodeBuf=new WCHAR[nUnicodeBufLen];
	if (pUnicodeBuf==0)
		return NULL;

	MultiByteToWideChar(GB2312_CODE_PAGE , 0, lpszGb32Text, -1, pUnicodeBuf, nUnicodeBufLen);

	int nUtf8BufLen=WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuf, -1, 0, 0, NULL, NULL);
	if (nUtf8BufLen==0)
	{
		delete[] pUnicodeBuf;
		return NULL;
	}

	char* pUft8Buf=new char[nUtf8BufLen];
	if (pUft8Buf==0)
	{
		delete[] pUnicodeBuf;
		return NULL;
	}

	WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuf, -1, pUft8Buf, nUtf8BufLen, NULL, NULL);

	return pUft8Buf;
}

UINT AnalyseDbNullTerminatedText(LPCTSTR lpszFormattedInfo,CStdArray& InfoArray)
{
	InfoArray.RemoveAll();
	if (lpszFormattedInfo==NULL)
		return 0;

	while(*lpszFormattedInfo != '\0')
	{
		InfoArray.Add(lpszFormattedInfo);
		lpszFormattedInfo+=_tcslen(lpszFormattedInfo)+1;
	}

	return InfoArray.GetSize();
}

void AnalyseFormattedTextInfoItem(LPCTSTR lpszFormattedInfo, CStdArray& InfoArray, TCHAR ch)
{
	InfoArray.RemoveAll();

	while(TRUE)
	{
		LPCTSTR lpszSeparator = _tcschr(lpszFormattedInfo, ch);
		if (lpszSeparator)
			InfoArray.Add(CString(lpszFormattedInfo, (int)(lpszSeparator-lpszFormattedInfo)));
		else
		{
			lpszSeparator = lpszFormattedInfo+_tcslen(lpszFormattedInfo);
			if (lpszSeparator!=lpszFormattedInfo)
				InfoArray.Add(CString(lpszFormattedInfo, (int)(lpszSeparator-lpszFormattedInfo)));
			break;
		}
		lpszFormattedInfo = lpszSeparator+1;
	}
}

BOOL QXUtf8ToUnicode(LPCSTR lpszUTF8, LPTSTR lpwText, DWORD& dwSize)
{  
	BOOL bSuccess = FALSE;
	do 
	{
		if (lpszUTF8 == NULL || lpszUTF8[0] == NULL)
		{  
			dwSize = 0; 
			break;
		}

		DWORD dwLen = MultiByteToWideChar(CP_UTF8, 0, lpszUTF8, -1, NULL, 0);  

		if (dwSize < dwLen+1)
		{
			dwSize = dwLen + 1;
			return FALSE;
		}

		MultiByteToWideChar(CP_UTF8, 0, lpszUTF8, -1, lpwText, dwLen);  
		lpwText[dwLen] = L'\0';
		dwSize = dwLen; 

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
} 

BOOL QXUnicodeToUtf8(LPCWSTR lpwText, LPSTR lpUTF8, DWORD& dwSize)
{  
	BOOL bSuccess = FALSE;
	do 
	{
		if (lpwText == NULL || lpwText[0] == NULL)
		{  
			dwSize = 0; 
			break;
		} 

		DWORD dwLen = WideCharToMultiByte(CP_UTF8, 0, lpwText, -1, NULL, 0, NULL, NULL);  
		if (dwSize < dwLen +1)
		{
			dwSize =dwLen + 1;
			return FALSE;
		}

		WideCharToMultiByte(CP_UTF8, 0, lpwText, -1, lpUTF8, dwLen, NULL,NULL); 
		lpUTF8[dwLen] = '\0';

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}  
