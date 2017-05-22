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

BOOL URLEncode(LPCTSTR lpszSrcURL, CString& strEncodeURL)
{
	if (lpszSrcURL == NULL)
		return FALSE;

	CStringW strSrcURL(lpszSrcURL);
	DWORD dwLen = strSrcURL.GetLength();

	if (dwLen == 0)
		return TRUE;

	int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, strSrcURL, dwLen, NULL, 0, NULL, NULL);
	LPSTR lpUTF8 = (LPSTR) new BYTE[cbUTF8+1];
	if (lpUTF8 == NULL)
		return FALSE;

	WideCharToMultiByte(CP_UTF8, 0, strSrcURL, dwLen, lpUTF8, cbUTF8 + 1, NULL, NULL);
	lpUTF8[cbUTF8] = '\0';

	//经过URLEncode编码的字符串内容
	BYTE bData[4096];
	BYTE lpByte;
	DWORD cbDest = 0; //累加
	LPSTR lpUTF8URL = lpUTF8;
	LPSTR lpDstURL = (LPSTR)bData;
	DWORD dwDataSize = _countof(bData);
	while(*lpUTF8URL && cbDest < dwDataSize - 1)
	{
		lpByte = *lpUTF8URL;
		if (lpByte == ' ')
		{
			*lpDstURL = '+';
			++lpDstURL;
			++cbDest;
		}
		else if (isalpha(lpByte) || isdigit(lpByte) || lpByte < 0x80)	//大概包含'-','.','~',':','/','=','&'等字符
		{
			*lpDstURL = lpByte;
			++lpDstURL;
			++cbDest;
		}
		else
		{
			//检查缓冲区大小是否够用？
			if(cbDest + 3 > dwDataSize - 1)
				break;
			lpDstURL[0] = '%';
			lpDstURL[1] = (lpByte >= 0xA0) ? ((lpByte >> 4) - 10 + 'A') : ((lpByte >> 4) + '0');
			lpDstURL[2] = ((lpByte & 0xF) >= 0xA)? ((lpByte & 0xF) - 10 + 'A') : ((lpByte & 0xF) + '0');
			lpDstURL += 3;
			cbDest += 3;
		}
		++lpUTF8URL;
	}

	*lpDstURL = '\0';
	delete[] lpUTF8;
	lpUTF8 = NULL;

	strEncodeURL = (LPSTR)bData;

	return TRUE;
}

BOOL URLDecode(LPCSTR lpszEncodeURL, CString& strDecodeURL)
{
	strDecodeURL.Empty();
	if (lpszEncodeURL == NULL || strlen(lpszEncodeURL) == 0)
		return FALSE;

	BYTE bData[4096];
	int cbDest = 0; //累加
	LPCSTR lpszSrcURL = lpszEncodeURL;
	LPSTR lpDstURL = (LPSTR)bData;
	while(*lpszSrcURL)
	{
		if(*lpszSrcURL == '%')
		{
			*lpDstURL = 0;
			//高位
			if(lpszSrcURL[1] >= 'A' && lpszSrcURL[1] <= 'F')
				*lpDstURL += (lpszSrcURL[1] - 'A' + 10) * 0x10;
			else if(lpszSrcURL[1] >= 'a' && lpszSrcURL[1] <= 'f')
				*lpDstURL += (lpszSrcURL[1] - 'a' + 10) * 0x10;
			else
				*lpDstURL += (lpszSrcURL[1] - '0') * 0x10;

			//低位
			if(lpszSrcURL[2] >= 'A' && lpszSrcURL[2] <= 'F')
				*lpDstURL += (lpszSrcURL[2] - 'A' + 10);
			else if(lpszSrcURL[2] >= 'a' && lpszSrcURL[2] <= 'f')
				*lpDstURL += (lpszSrcURL[2] - 'a' + 10);
			else
				*lpDstURL += (lpszSrcURL[2] - '0');

			lpszSrcURL += 3;
		}
		else if(*lpszSrcURL == '+')
		{
			*lpDstURL = ' ';
			++lpszSrcURL;
		}
		else
		{
			*lpDstURL = *lpszSrcURL;
			++lpszSrcURL;
		}
		++lpDstURL;
		++cbDest;
	}
	//null-terminator
	*lpDstURL = '\0';
	++cbDest;

	LPWSTR lpwText = new WCHAR[cbDest];
	if (lpwText == NULL)
		return FALSE;

	DWORD dwSize = cbDest;
	QXUtf8ToUnicode((LPCSTR)bData, lpwText, dwSize);

	strDecodeURL = (LPCWSTR)lpwText;
	delete[] lpwText;
	lpwText = NULL;

	return TRUE;
}