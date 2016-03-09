#include "stdafx.h"
#include "Md5.h"
#include "Crc32.h"
#include "Sha1.h"
#include "..\ErrorInfo.h"
#include "..\DebugInfo.h"

CHash* CHash::HashType(int nHashType)
{
	CHash *pHash=0;	 
	switch(nHashType)
	{
	case HASH_TYPE_MD5:
		pHash = new CMD5;
		break; 
	case HASH_TYPE_CRC32:
		pHash = new CCRC32;
		break;
	case HASH_TYPE_SHA1:
        pHash = new CSHA1;
		break;
	default:
		SetErrorInfo(CUSTOM_ERROR,0,_T("不支持的Hash算法(%d)."),nHashType);
		return 0;
	}
	return pHash;
}

void CHash::GetHash(int nHashType, BYTE* pBuffer, DWORD dwSize, LPTSTR szHashText)
{
	BYTE buffer[MAX_PATH]={0};
	CHash* pHash = CHash::HashType(nHashType);
	pHash->Init();
	pHash->Update(pBuffer, dwSize);
	pHash->Final(buffer);
	pHash->GetHashHexText(buffer, szHashText);
	pHash->Release();
}

DWORD CHash::GetHash(int nHashType, BYTE* pBuffer, DWORD dwSize, BYTE* pHashvalue)
{
	CHash* pHash = CHash::HashType(nHashType);
	pHash->Init();
	pHash->Update(pBuffer, dwSize);
	DWORD dwRet=pHash->Final(pHashvalue);
	pHash->Release();
	return dwRet;
}

BOOL CHash::GetFileHash(LPCTSTR lpcszName, BYTE* pHashvalue)
{
	HANDLE hFile = CreateFile(lpcszName, FILE_GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("计算文件校验时打开文件失败:"));
		return FALSE;
	}

	DWORD dwSizeHigh, dwSizeLow;
	dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
	ULONGLONG ullFileSize=dwSizeHigh;
	ullFileSize<<=32;
	ullFileSize+=dwSizeLow;
	const int nBufferSize= 128*1024;   
	BYTE byBuffer[nBufferSize] = {0};
	DWORD dwGetLen = nBufferSize;      //文件中要读取数据大小
	DWORD dwOutLen;           //从文件中读出数据大小
	Init();
    while(ullFileSize>0)
	{
		if(ReadFile(hFile, byBuffer, dwGetLen, &dwOutLen, NULL)==FALSE)
		{
			CloseHandle(hFile);
			SetErrorInfo(SYSTEM_ERROR,0,_T("读取文件时出错:"));
			return FALSE;
		}
		else
		{
			Update(byBuffer, dwOutLen);
			ullFileSize -= dwOutLen;
		}
	}
	Final(pHashvalue);
	CloseHandle(hFile);

	return TRUE;
}

void CHash::GetBigEndianResult(BYTE* pHashValue)
{
}

void CHash::GetHashHexText(const BYTE* pHashBuffer,LPTSTR szHashText)
{
	CString strHashText;
	GetHashHexText(pHashBuffer,strHashText);
	_tcscpy_s(szHashText,m_nHashValueLen,strHashText);
}

void CHash::GetHashHexText(const BYTE* pHashBuffer,CString& strHashText)
{
	CString strHash;
	for (int i=0; i<m_nHashValueLen; i++)
	{
		strHash.Format(_T("%02X"),pHashBuffer[i]);
		strHashText += strHash;
	}
}

void CHash::Release()
{
	delete this;
}


