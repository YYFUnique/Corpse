#include "stdafx.h"
#include "LsRc4.h"
#include "../Utils/ErrorInfo.h"

void CLsRc4::Init(const BYTE* lpKey , DWORD dwKeyLen)
{
    for(int i=0;i<256;i++) 
        m_Data[i]=i;

	int j=0;
    for(int i=0;i<256;i++)
	{
        j = (j+m_Data[i]+lpKey[i%dwKeyLen])%256;

        BYTE bTmp = m_Data[i];
        m_Data[i]    = m_Data[j];//交换s[i]和s[j]
        m_Data[j]    = bTmp;
    }
}

void CLsRc4::Crypt(const BYTE* lpInput , BYTE* lpOutput , DWORD dwDataLen)
{
	int i=0,j=0,t=0;
	BYTE bTmp;
    for(UINT n=0; n<dwDataLen; ++n)
    {
        i=(i+1)%256;
        j=(j+m_Data[i])%256;

        bTmp=m_Data[i];
        m_Data[i]=m_Data[j];//交换s[x]和s[y]
        m_Data[j]=bTmp;

        t=(m_Data[i]+m_Data[j])%256;
        lpOutput[n]=lpInput[n] ^ m_Data[t];
    }
}

BOOL CLsRc4::CryptFile(LPCTSTR lpszInputFile ,DWORD dwHeadLen , LPCTSTR lpszOutputFile , const void* lpszFilePwd , DWORD dwPwdLen)
{
	BOOL bSuccess = FALSE;
	HANDLE hInputFile = INVALID_HANDLE_VALUE , hOutputFile = INVALID_HANDLE_VALUE;
	BYTE* lpFileBuffer = NULL;

	do
	{
		CLsRc4 Rc4;
		Rc4.Init((BYTE*)lpszFilePwd , dwPwdLen);

		hInputFile = CreateFile(lpszInputFile , GENERIC_READ , FILE_SHARE_READ , NULL , OPEN_EXISTING , 0 , NULL);
		if (hInputFile == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("打开源文件失败"));
			break;
		}

		LARGE_INTEGER FileSize;
		if (GetFileSizeEx(hInputFile , &FileSize) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("获取源文件大小失败"));
			break;
		}

		const DWORD dwBufferLen = 1024*1024;
		lpFileBuffer = new BYTE[dwBufferLen];
		if (lpFileBuffer == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR , 0 , _T("加密文件失败:分配内存失败."));
			break;
		}

		hOutputFile = CreateFile(lpszOutputFile , GENERIC_WRITE , FILE_SHARE_READ , NULL , CREATE_ALWAYS , 0 , NULL);
		if (hOutputFile == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("打开目标文件失败"));
			break;
		}

		SetFilePointer(hInputFile,dwHeadLen,NULL,FILE_BEGIN);

		LARGE_INTEGER FileOffset;
		FileOffset.QuadPart = 0;

		while(FileOffset.QuadPart < FileSize.QuadPart)
		{
			DWORD dwFileReadLen = dwBufferLen;
			if (FileSize.QuadPart - FileOffset.QuadPart < dwBufferLen)
				dwFileReadLen = (DWORD)(FileSize.QuadPart - FileOffset.QuadPart);

			DWORD dwFileReadWrite;
			if (ReadFile(hInputFile , lpFileBuffer , dwFileReadLen , &dwFileReadWrite , NULL) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR , 0 , _T("读取源文件失败"));
				break;
			}

			Rc4.Crypt(lpFileBuffer , lpFileBuffer , dwFileReadWrite);

			if (WriteFile(hOutputFile , lpFileBuffer , dwFileReadWrite , &dwFileReadWrite , NULL) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR , 0 , _T("写目标文件失败"));
				break;
			}

			FileOffset.QuadPart += dwFileReadLen;
		}

		if (FileOffset.QuadPart == FileSize.QuadPart)
			bSuccess = TRUE;
	}while(0);

	if (lpFileBuffer)
		delete[] lpFileBuffer;
	if (hInputFile != INVALID_HANDLE_VALUE)
		CloseHandle(hInputFile);
	if (hOutputFile != INVALID_HANDLE_VALUE)
		CloseHandle(hOutputFile);

	return bSuccess;
}