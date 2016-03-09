#include "stdafx.h"
#include "LscLibEncrypt.h"
#include "Rijndael.h"

/*
特殊说明:由于此处是为了解决因为外部对象在其构造函数调用之前该CPP中
的某些全局变量未初始化而导致调用非法的错误,可以在外部调用之前率先
初始化此CPP中的全局变量
*/
#ifdef LS_STATIC_LIB_CALL
#pragma warning(disable:4073)
#pragma init_seg(lib)
#endif

CRijndael g_rijndael;

unsigned char g_arrKey[48] = {0x3D, 0xBA, 0x59, 0x1D, 0x72, 0xE0, 0x5F, 0x9A, 0x49, 0x25, 0x32,
	0x55, 0xC4, 0xFC, 0x54, 0x49, 0x94,0x6C, 0xC8, 0xC2, 0x28, 0x29, 0x91, 0x92, 0xBB, 0xC7,
	0xA4, 0xF5, 0xD8, 0x7E, 0x44,0x90, 0x21, 0x4F, 0x3C, 0x3D, 0xF9, 0x7A, 0xBA, 0xEB, 0x0F,
	0x91, 0xD3, 0xEC, 0xC3, 0xFE, 0xC3, 0xBF};

BOOL LibEncryptFile(LPCTSTR lpszSrcFile, LPCTSTR lpszDestFile)
{
	HANDLE hSrcFile, hDestFile;
	hSrcFile = CreateFile(lpszSrcFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					0, NULL);
	if(hSrcFile == INVALID_HANDLE_VALUE)
		return FALSE;
	hDestFile = CreateFile(lpszDestFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL, NULL);
	if(hDestFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSrcFile);
		return FALSE;
	}

	BOOL bReturn = FALSE;
	DWORD dwSrcFileSize = GetFileSize(hSrcFile, NULL);
	if(dwSrcFileSize != INVALID_FILE_SIZE && dwSrcFileSize > 0)
	{
		LPBYTE lpSrcBuff = new BYTE[dwSrcFileSize];
		LPBYTE lpDestBuff = new BYTE[dwSrcFileSize];

		DWORD dwRead;
		if(ReadFile(hSrcFile, lpSrcBuff, dwSrcFileSize, &dwRead, NULL) && dwRead == dwSrcFileSize)
		{
			g_rijndael.SetKey(g_arrKey, g_arrKey + 32, 32);
			if(g_rijndael.Encrypt(lpSrcBuff, lpDestBuff, dwSrcFileSize, CRijndael::CBC))
			{
				DWORD dwWritten;
				if(WriteFile(hDestFile, lpDestBuff, dwSrcFileSize, &dwWritten, NULL)
						&& dwWritten == dwSrcFileSize)
				{
					bReturn = TRUE;
				}
			}
		}

		delete[] lpDestBuff;
		delete[] lpSrcBuff;
	}

	CloseHandle(hDestFile);
	CloseHandle(hSrcFile);

	return bReturn;
}

LPSTR LibDecrypt(LPCTSTR lpszEncFile)
{
	HANDLE hEncFile = CreateFile(lpszEncFile, GENERIC_READ, FILE_SHARE_READ, NULL,
							OPEN_EXISTING, 0, NULL);
	if(hEncFile == INVALID_HANDLE_VALUE)
		return NULL;

	LPSTR lpReturn = NULL;
	DWORD dwEncFileSize = GetFileSize(hEncFile, NULL);
	if(dwEncFileSize != INVALID_FILE_SIZE && dwEncFileSize > 0)
	{
		LPBYTE lpInBuff = new BYTE[dwEncFileSize];
		LPBYTE lpOutBuff = new BYTE[dwEncFileSize + 1];

		DWORD dwRead;
		if(ReadFile(hEncFile, lpInBuff, dwEncFileSize, &dwRead, NULL) && dwRead == dwEncFileSize)
		{
			g_rijndael.SetKey(g_arrKey, g_arrKey + 32, 32);
			if(g_rijndael.Decrypt(lpInBuff, lpOutBuff, dwEncFileSize + 1, CRijndael::CBC))
			{
				lpOutBuff[dwEncFileSize] = '\0';
				lpReturn = (LPSTR)lpOutBuff;
			}
		}

		//delete[] lpOutBuff; // 不能释放，用作返回值
		delete[] lpInBuff;
	}

	CloseHandle(hEncFile);

	return lpReturn;
}

void LibBufferFree(void *pBuffer)
{
	if(pBuffer)
		delete[] pBuffer;
}