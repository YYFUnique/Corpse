#include "stdafx.h"
#include "PELibrary.h"
#include "LsCommon/ErrorInfo.h"

CPELibrary::CPELibrary()
{
	ZeroMemory(&m_ImageDosHeader,sizeof(IMAGE_DOS_HEADER));
	ZeroMemory(&m_ImageNtHeader,sizeof(IMAGE_NT_HEADERS32));
}

CPELibrary::~CPELibrary()
{

}

BOOL CPELibrary::ParsePEInfo(LPCTSTR lpszFilePath)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = NULL;
	LPVOID lpMemory = NULL;
	HANDLE hMapFile = NULL;

	do 
	{
		m_strFileName = PathFindFileName(lpszFilePath);

		hFile = CreateFile(lpszFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开文件 %s 失败"),lpszFilePath);
			break;
		}
		
		if (GetFileSize(hFile,NULL) == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取文件 %s 大小失败"),lpszFilePath);
			break;
		}

		hMapFile = CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);
		if (hMapFile == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("映射内存文件 %s 失败"),lpszFilePath);
			break;
		}
		
		lpMemory = MapViewOfFile(hMapFile,FILE_MAP_READ,NULL,NULL,NULL);
		if (IsPeFile(lpMemory) == FALSE)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("文件 %s 不是PE格式文件！"),lpszFilePath);
			break;
		}

		memcpy_s(&m_ImageDosHeader,sizeof(IMAGE_DOS_HEADER),lpMemory,sizeof(IMAGE_DOS_HEADER));
		memcpy_s(&m_ImageNtHeader,sizeof(IMAGE_NT_HEADERS32),((LPBYTE)lpMemory+m_ImageDosHeader.e_lfanew),sizeof(IMAGE_NT_HEADERS32));
		
		bSuccess = TRUE;
	} while (FALSE);
	
	if (lpMemory != NULL)
		UnmapViewOfFile(lpMemory);

	if (hMapFile != INVALID_HANDLE_VALUE && hFile != NULL)
		CloseHandle(hMapFile);

	if (hFile != INVALID_HANDLE_VALUE && hFile != NULL)
		CloseHandle(hFile);

	return bSuccess;
}

BOOL CPELibrary::IsPeFile(LPVOID pImageBase)
{
	PIMAGE_DOS_HEADER    pDosHeader = NULL;
	PIMAGE_NT_HEADERS    pNtHeader = NULL;
	BOOL bSuccess = FALSE;
	do 
	{
		if(pImageBase == NULL)
			break;

		pDosHeader = (PIMAGE_DOS_HEADER) pImageBase;

		if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			break;

		pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);
		if(pNtHeader->Signature != IMAGE_NT_SIGNATURE )
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

CString& CPELibrary::GetFileName()
{
	return m_strFileName;
}

IMAGE_DOS_HEADER& CPELibrary::GetImageDosHeader()
{
	return m_ImageDosHeader;
}

IMAGE_NT_HEADERS& CPELibrary::GetImageNtHeader()
{
	return m_ImageNtHeader;
}