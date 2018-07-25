#include "StdAfx.h"
#include "Convert.h"

//当type为CP_ACP时，UNICODE转化为GBK；当type为CP_UTF8时，UNICODE转化为UTF8
BOOL Transform(LPCWSTR lpszwch, int& nSize, LPSTR lpszConvert, int nType/* = CP_ACP*/) 
{
	BOOL bSuccess = FALSE;
	do 
	{
		int nLen = WideCharToMultiByte(nType, 0, lpszwch, -1, NULL, 0, NULL, NULL);
		if (nLen > nSize)
		{
			nSize = nLen + 1;
			break;
		}
		
		WideCharToMultiByte(nType, 0, lpszwch, -1, lpszConvert, nLen, NULL, NULL);
		lpszConvert[nLen] = '\0';
		
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL Transform(LPCSTR lpszch, int& nSize, LPWSTR lpszConvert, int nType/* = CP_ACP*/) 
{
	BOOL bSuccess = FALSE;
	do 
	{
		int nLen = MultiByteToWideChar(nType, 0, lpszch, -1, NULL, 0);
		if (nLen > nSize)
		{
			nSize = nLen + 1;
			break;
		}
		
		MultiByteToWideChar(nType, 0, lpszch, -1, lpszConvert, nLen);
		lpszConvert[nLen] = L'\0';

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}