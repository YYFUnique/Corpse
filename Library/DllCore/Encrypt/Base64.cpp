#include "stdafx.h"
#include "Base64.h"
#include "../Utils/ErrorInfo.h"
#include "../Utils/TextTools.h"

BOOL Base64Encode(LPCTSTR lpszSrcData,int nSrcLen,LPTSTR szDest,int *pnDestLen,DWORD dwFlags) 
{
	static const TCHAR szBase64EncodingTable[65] = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	
	int nWritten = 0;
	DWORD dwValue = 0;
	LPCTSTR lpbInput = lpszSrcData;
	
	int nMultiple3 = nSrcLen - (nSrcLen%3);
	int nRemain = (nSrcLen%3) ? (nSrcLen%3) : 0;
	int nPad = nRemain ? 3-nRemain : 0;
	
	// 3 的整数倍
	for (int i=3; i<=nMultiple3; i+=3)
	{
		for (int j=0; j<3; j++)
		{
			dwValue |= *lpbInput++;
			dwValue <<= 8;
		}
		
		for (int k=0; k<4; k++)
		{
			*szDest++ = szBase64EncodingTable[(BYTE)(dwValue>>26)];
			dwValue <<= 6;
		}
		
		nWritten += 4;
		
		if (i != 0 && (i % 48) == 0 && (dwFlags & ATL_BASE64_FLAG_NOCRLF) == 0)
		{
			*szDest++ = _T('\r');
			*szDest++ = _T('\n');
			nWritten+= 2;
		}
		
		dwValue = 0;
	}
	
	// 剩余字节
	if (nRemain > 0)
	{
		for (int r=0; r<3; r++)
		{
			if (r<nRemain)
			{
				dwValue |= *lpbInput++;
			}
			dwValue <<= 8;
		}
		
		for (int p=0; p<(nRemain+1); p++)
		{
			*szDest++ = szBase64EncodingTable[(BYTE)(dwValue>>26)];
			dwValue <<= 6;
		}
		nWritten += nRemain+1;
	}
	
	if ((dwFlags & ATL_BASE64_FLAG_NOPAD) == 0)
	{
		for(int q=0; q<nPad; q++)
		{
			*szDest++ = _T('=');
			nWritten++;
		}	
	}
	
	if (nWritten != 0 && (dwFlags & ATL_BASE64_FLAG_NOCRLF) == 0)
	{
		*szDest++ = _T('\r');
		*szDest++ = _T('\n');
		nWritten+= 2;
	}

	if (nWritten < *pnDestLen)
		szDest[nWritten] = _T('\0');

	*pnDestLen = nWritten;

	return TRUE;
}

BOOL URLEncode(LPCWSTR lpwURL, CString& strCryptURL)
{
	CHAR szCryptURL[4*1024];
	CHAR szUTF8Code[4*1024 ];
	DWORD dwSize = _countof(szUTF8Code);
	QXUnicodeToUtf8(lpwURL, szUTF8Code, dwSize);

	int n=0;
	DWORD dwLen = 0;
	while(szUTF8Code[n] != NULL)
	{
		TCHAR szEncode[4];
		ZeroMemory(szEncode, _countof(szEncode));
		CHAR szCode = szUTF8Code[n];
		if (isalpha(szCode) || isdigit(szCode) || szCode == ':' || szCode == '&' || szCode == '=' || szCode == '-' || szCode == '.' || szCode == '~' || szCode == '/' || szCode == '?')
			szCryptURL[dwLen++] = szUTF8Code[n];
		else if (szCode == ' ')
		{
			szCryptURL[dwLen++] = '+';
		}
		else
		{
			szCryptURL[dwLen++] = '%';
			szCryptURL[dwLen++] = (szCode >= 0xA0) ? ((szCode >> 4) - 10 + 'a') : ((szCode >> 4) + '0');
			szCryptURL[dwLen++] = ((szCode & 0xF) >= 0xA)? ((szCode & 0xF) - 10 + 'a') : ((szCode & 0xF) + '0');
		}
		++n;
	}

	szCryptURL[n] = '\0';
	strCryptURL = szCryptURL;

	return TRUE;
}