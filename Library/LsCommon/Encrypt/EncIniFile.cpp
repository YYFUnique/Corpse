// LsEncIniFile.cpp : implementation file
//

#include "stdafx.h"
#include "EncIniFile.h"
#include "..\ErrorInfo.h"
#include "..\DebugInfo.h"
#include "..\Hash\Hash.h"
#include "EncryptObject.h"
#include "Rijndael.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
特殊说明:由于此处是为了解决因为外部对象在其构造函数调用之前该CPP中
的某些全局变量未初始化而导致调用非法的错误,可以在外部调用之前率先
初始化此CPP中的全局变量
*/
#ifdef LS_STATIC_LIB_CALL
#pragma warning(disable:4073)
#pragma init_seg(lib)
#endif

const BYTE CEncIniFile::m_byEncryptKeyIv[48]={0x4B, 0x24, 0x7C, 0x23, 0x19, 0x43, 0x28, 0xA7, 0x08, 0x57, 0xCC, 0xE8, 0x8E, 0x21, 0x0F, 0x3A, 
		0x64, 0x5C, 0xF8, 0x7A, 0x4C, 0xD3, 0x67, 0x28, 0x0A, 0x7A, 0x64, 0x8E, 0x9D, 0xAF, 0x63, 0xD5, 
		0x00, 0x2C, 0xF1, 0x12, 0xFF, 0x65, 0x77, 0xC2, 0xEE, 0x4C, 0xF9, 0x35, 0x4C, 0xB6, 0x03, 0xDD};

/////////////////////////////////////////////////////////////////////////////
// CLsEncIniFile

CEncIniFile::CEncIniFile()
:CFileDataItem()
{
}

CEncIniFile::~CEncIniFile()
{
}

CString& CEncIniFile::GetFileName()
{
	return m_strFileName;
}

BOOL CEncIniFile::SaveDataToFile(DWORD dwFileVersion,LPCTSTR lpszFileName,LPCTSTR lpszMagicText)
{
	HANDLE hFile=INVALID_HANDLE_VALUE;
	BYTE* pFileBuffer=NULL;
	BOOL bSuccess=FALSE;
	do
	{
		if (dwFileVersion!=1)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("保存配置文件设置的版本号不支持!"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}

		hFile=CreateFile(lpszFileName,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,0,0);
		if (hFile==INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("保存配置文件时打开文件失败:"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}

		DWORD dwFileHeadLen=0;
		if (dwFileVersion==1)
			dwFileHeadLen=(DWORD)_tcslen(lpszMagicText)+sizeof(DWORD)+sizeof(DWORD)+sizeof(DWORD);//magic+ver+body len+crc32

		DWORD dwDataLen;
		GetAllDataBuffer(0,dwDataLen);
		pFileBuffer=new BYTE[dwDataLen+dwFileHeadLen];
		if (pFileBuffer==NULL)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("保存配置文件时分配内存(%d字节)失败!"),dwDataLen+dwFileHeadLen);
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}
		//获取文件体
		if (GetAllDataBuffer(pFileBuffer+dwFileHeadLen,dwDataLen)==FALSE)
			break;
		
		//填充文件头
		if (dwFileVersion==1)
		{
			DWORD dwMagicTextLen = _tcslen(lpszMagicText);
			_tcscpy_s((LPTSTR)pFileBuffer,dwMagicTextLen*sizeof(TCHAR),lpszMagicText);
			DWORD dwOffset = dwMagicTextLen;

			*(DWORD*)(pFileBuffer+dwOffset)=dwFileVersion;
			dwOffset+=sizeof(DWORD);

			*(DWORD*)(pFileBuffer+dwOffset)=dwDataLen;
			dwOffset+=sizeof(DWORD);

			CHash::GetHash(HASH_TYPE_CRC32,pFileBuffer+dwFileHeadLen,dwDataLen,pFileBuffer+dwOffset);
			dwOffset+=sizeof(DWORD);
		}

		CEncryptObject* pEncryptObject=CEncryptObject::Create(LS_ENCRYPT_ALG_AES);
		if (pEncryptObject)
		{
			pEncryptObject->SetKey(m_byEncryptKeyIv,m_byEncryptKeyIv+32,32,16);
			pEncryptObject->Encrypt(pFileBuffer+dwFileHeadLen-sizeof(DWORD),0,dwDataLen+sizeof(DWORD),CEncryptObject::CBC);
			pEncryptObject->Release();
		}

		DWORD dwNumberOfBytesWritten;
		if (WriteFile(hFile,pFileBuffer,dwDataLen+dwFileHeadLen,&dwNumberOfBytesWritten,0)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("保存配置文件时写文件失败:"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}

		m_strFileName=lpszFileName;
		bSuccess=TRUE;
	} while (FALSE);

	if (pFileBuffer)
		delete[] pFileBuffer;

	if (hFile!=INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return bSuccess;
}

BOOL CEncIniFile::LoadDataFromFile(LPCTSTR lpszFileName,LPCTSTR lpszMagicText)
{
	HANDLE hFile=INVALID_HANDLE_VALUE;
	BYTE* pFileBuffer=NULL;
	BOOL bSuccess=FALSE;
	do
	{
		ClearAllDataItem();

		DWORD dwMagicTextLen=(DWORD)_tcslen(lpszMagicText);
		if (dwMagicTextLen>512)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("读取配置文件时设置的标示信息超出长度(%d字节)!"),dwMagicTextLen);
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}

		hFile=CreateFile(lpszFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
		if (hFile==INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("读取配置文件时打开文件失败:"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}
		
		BYTE byFileHead[1024];
		DWORD dwFileHeadLen=dwMagicTextLen+sizeof(DWORD)+sizeof(DWORD)+sizeof(DWORD);//magic+ver+body len+crc32

		LARGE_INTEGER liFileSize;
		if (GetFileSizeEx(hFile,&liFileSize)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("读取配置文件时获取文件大小失败:"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}
		else if (liFileSize.QuadPart<dwFileHeadLen)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("读取配置文件时文件大小不正确!"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}
		
		DWORD dwNumberOfBytesRead;
		if (ReadFile(hFile,byFileHead,dwFileHeadLen,&dwNumberOfBytesRead,0)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("读取配置文件头失败:"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}

		if (memcmp(byFileHead,lpszMagicText,dwMagicTextLen))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("读取配置文件时文件头标志校验失败!"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}

		DWORD dwFileVersion=*(DWORD*)(byFileHead+dwMagicTextLen);
		if (dwFileVersion!=1)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("读取配置文件时文件版本不支持!"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}
		
		DWORD dwFileBodySize=*(DWORD*)(byFileHead+dwMagicTextLen+sizeof(DWORD));
		DWORD dwEncryptFileBodyCrc32=*(DWORD*)(byFileHead+dwMagicTextLen+sizeof(DWORD)+sizeof(DWORD));

		pFileBuffer=new BYTE[dwFileBodySize+sizeof(DWORD)];
		if (pFileBuffer==NULL)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("读取配置文件时分配内存失败(%d字节)!"),dwFileBodySize);
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}
		*(DWORD*)pFileBuffer=dwEncryptFileBodyCrc32;
		if (ReadFile(hFile,pFileBuffer+sizeof(DWORD),dwFileBodySize,&dwNumberOfBytesRead,0)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("读取配置文件体失败:"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}

		CEncryptObject* pEncryptObject=CEncryptObject::Create(LS_ENCRYPT_ALG_AES);
		if (pEncryptObject)
		{
			pEncryptObject->SetKey(m_byEncryptKeyIv,m_byEncryptKeyIv+32,32,16);
			pEncryptObject->Decrypt(pFileBuffer,0,dwFileBodySize+sizeof(DWORD),CEncryptObject::CBC);
			pEncryptObject->Release();
		}

		//判断校验
		DWORD dwNowFileBodyCrc32;
		CHash::GetHash(HASH_TYPE_CRC32,pFileBuffer+sizeof(DWORD),dwFileBodySize,(BYTE*)&dwNowFileBodyCrc32);
		if (dwNowFileBodyCrc32!=*(DWORD*)pFileBuffer)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("读取配置文件体失败:文件校验不正确!"));
			OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
			break;
		}

		DWORD dwOffset=sizeof(DWORD);
		while(dwOffset<dwFileBodySize+sizeof(DWORD))
		{
			DWORD dwDataType=*(DWORD*)(pFileBuffer+dwOffset);
			dwOffset+=sizeof(DWORD);

			DWORD dwDataLen=*(DWORD*)(pFileBuffer+dwOffset);
			dwOffset+=sizeof(DWORD);

			if (dwDataLen+dwOffset>dwFileBodySize+sizeof(DWORD))
				break;

			SetDataItem(dwDataType,dwDataLen,pFileBuffer+dwOffset);
			dwOffset+=dwDataLen;
		}

		m_dwFileVersion=dwFileVersion;
		m_strFileName=lpszFileName;
		bSuccess=TRUE;
	} while (FALSE);
	
	if (pFileBuffer)
		delete[] pFileBuffer;
	
	if (hFile!=INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	
	return bSuccess;
}