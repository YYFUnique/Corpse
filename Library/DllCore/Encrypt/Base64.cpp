#include "stdafx.h"
#include "Base64.h"
#include "../Utils/ErrorInfo.h"

BOOL Base64Encode(const BYTE *pbSrcData,int nSrcLen,LPSTR szDest,int *pnDestLen,DWORD dwFlags) 
{
	static const char szBase64EncodingTable[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
	int nWritten = 0;
	DWORD dwValue = 0;
	PBYTE pbInput = (PBYTE)pbSrcData;
	
	int nMultiple3 = nSrcLen - (nSrcLen%3);
	int nRemain = (nSrcLen%3) ? (nSrcLen%3) : 0;
	int nPad = nRemain ? 3-nRemain : 0;
	
	// 3 的整数倍
	for (int i=3; i<=nMultiple3; i+=3)
	{
		for (int j=0; j<3; j++)
		{
			dwValue |= *pbInput++;
			dwValue <<= 8;
		}
		
		for (int k=0; k<4; k++)
		{
			*szDest++ = szBase64EncodingTable[(BYTE)(dwValue>>26)];
			dwValue <<= 6;
		}
		
		nWritten += 4;
		
		if (i != 0 && (i % 48) == 0 
			&& (dwFlags & ATL_BASE64_FLAG_NOCRLF) == 0)
		{
			*szDest++ = '\r';
			*szDest++ = '\n';
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
				dwValue |= *pbInput++;
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
			*szDest++ = '=';
			nWritten++;
		}	
	}
	
	if (nWritten != 0 
		&& (dwFlags & ATL_BASE64_FLAG_NOCRLF) == 0)
	{
		*szDest++ = '\r';
		*szDest++ = '\n';
		nWritten+= 2;
	}
	
	*pnDestLen = nWritten;

	return TRUE;
}