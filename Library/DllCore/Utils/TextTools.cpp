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