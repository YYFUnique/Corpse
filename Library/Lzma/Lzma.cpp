#include "StdAfx.h"
#include "Lzma.h"
#include "xzip.h"
#include "xunzip.h"
#include <Shlwapi.h>
#include <atlstr.h>
#pragma comment(lib,"shlwapi.lib")

CLzma::CLzma()
{

}

CLzma::~CLzma()
{

}

BOOL CLzma::LzmaEncode(LPCTSTR lpszTargetFilePath,LPBYTE lpData,DWORD& dwLen)
{
	BOOL bSuccess = FALSE;
	HZIP hZip = NULL;
	do 
	{
		DWORD dwSize = 0;
		static TCHAR szTargetFilePath[MAX_PATH] = _T("\0");
		if (_tcsicmp(lpszTargetFilePath,szTargetFilePath) != 0)
		{
			dwSize = GetPathFileSizeLen(lpszTargetFilePath);
			if (dwLen < dwSize)
			{
				dwLen = dwSize;
				break;
			}

			_tcscpy_s(szTargetFilePath,_countof(szTargetFilePath),lpszTargetFilePath);
		}		

		hZip = CreateZip(0,dwLen,NULL);
		if (hZip == NULL)
			break;

		if (PathIsDirectory(lpszTargetFilePath))
			AddFileToZip(hZip,lpszTargetFilePath);
		else
			ZipAdd(hZip,PathFindFileName(lpszTargetFilePath),lpszTargetFilePath);

		LPVOID lpMemData = NULL;
		ZipGetMemory(hZip,(LPVOID*)&lpMemData,&dwSize);
		memcpy_s(lpData,dwLen,lpMemData,dwSize);
		dwLen = dwSize;

		bSuccess = TRUE;
	} while (FALSE);
	if (hZip != NULL)
		CloseZip(hZip);

	return bSuccess;
}

BOOL CLzma::LzmaEncode(LPCTSTR lpszTargetFilePath,LPCTSTR lpszBinFilePath)
{
	BOOL bSuccess = FALSE;
	HZIP hZip = NULL;
	do 
	{
		hZip = CreateZip(lpszBinFilePath,NULL);
		if (hZip == NULL)
			break;

		if (PathIsDirectory(lpszTargetFilePath))
			AddFileToZip(hZip,lpszTargetFilePath);
		else
			ZipAdd(hZip,PathFindFileName(lpszTargetFilePath),lpszTargetFilePath);

		bSuccess = TRUE;
	} while (FALSE);

	if (hZip != NULL)
		CloseZip(hZip);

	return bSuccess;
}

BOOL CLzma::LzmaDecode(LPCTSTR lpszTargetFilePath,LPCTSTR lpszPathFile)
{
	BOOL bSuccess = FALSE;
	HZIP hZip = NULL;
	do 
	{
		if (PathFileExists(lpszTargetFilePath) == FALSE)
			break;

		hZip = OpenZip(lpszTargetFilePath,NULL);
		if (hZip == NULL)
			break;
		if (GetFileFromZip(hZip,lpszPathFile) == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (hZip != NULL)
		CloseZip(hZip);

	return bSuccess;
}

BOOL CLzma::LzmaDecode(LPBYTE lpData,UINT nLen,LPCTSTR lpszPathFile)
{
	BOOL bSuccess = FALSE;
	HZIP hZip = NULL;
	do 
	{
		hZip = OpenZip((LPVOID)lpData,nLen,NULL);
		if (hZip == NULL)
			break;

		if (GetFileFromZip(hZip,lpszPathFile) == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (hZip != NULL)
		CloseZip(hZip);

	return bSuccess;
}

BOOL CLzma::AddFileToZip(HZIP hZip,LPCTSTR lpszTargetFilePath,LPCTSTR lpszRelativePath)
{
	WIN32_FIND_DATA Win32Data;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR szTargetFilePath[MAX_PATH];
	PathCombine(szTargetFilePath,lpszTargetFilePath,_T("*.*"));
	hFind = FindFirstFile(szTargetFilePath,&Win32Data);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	do 
	{
		if (_tcsicmp(Win32Data.cFileName,_T(".")) == 0 ||
			_tcsicmp(Win32Data.cFileName,_T("..")) == 0)
				continue;

		TCHAR szFilePath[MAX_PATH];
		PathCombine(szFilePath,lpszTargetFilePath,Win32Data.cFileName);
		
		if (Win32Data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			CString strRelativePath;
			if (_tcsicmp(lpszRelativePath,_T("")) == 0)
			{
				strRelativePath = Win32Data.cFileName;
				ZipAddFolder(hZip,Win32Data.cFileName);
			}
			else
			{
				strRelativePath.Format(_T("%s\\%s"),lpszRelativePath,Win32Data.cFileName);
				ZipAddFolder(hZip,strRelativePath);
			}
			AddFileToZip(hZip,szFilePath,strRelativePath);
		}
		else
		{
			if (_tcsicmp(lpszRelativePath,_T("")) == 0)
				ZipAdd(hZip,Win32Data.cFileName,szFilePath);
			else
			{
				CString strRelativePath;
				strRelativePath.Format(_T("%s\\%s"),lpszRelativePath,Win32Data.cFileName);
				ZipAdd(hZip,strRelativePath,szFilePath);
			}
		}

	} while (FindNextFile(hFind,&Win32Data));
	
	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	return TRUE;
}

BOOL CLzma::GetFileFromZip(HZIP hZip,LPCTSTR lpszPathDirectory)
{
	BOOL bSuccess = FALSE;
	do 
	{
		ZIPENTRY ZipEntry;
		GetZipItem(hZip,-1,&ZipEntry);
		TCHAR szTargetFilePath[MAX_PATH];
		UINT nCount = ZipEntry.index;

		for (UINT n=0;n<nCount;++n)
		{
			GetZipItem(hZip,n,&ZipEntry);
			PathCombine(szTargetFilePath,lpszPathDirectory,ZipEntry.name);
			UnzipItem(hZip,n,szTargetFilePath);
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

DWORD CLzma::GetPathFileSizeLen(LPCTSTR lpszTargetFilePath)
{
	DWORD dwSize = 0;
	do 
	{
		if (PathIsDirectory(lpszTargetFilePath))
			dwSize += GetFileSizeFromDirectory(lpszTargetFilePath);
		else
			dwSize += GetFileSize(lpszTargetFilePath);
	} while (FALSE);

	return dwSize;
}

DWORD CLzma::GetFileSizeFromDirectory(LPCTSTR lpszDirectoryPathFile)
{
	WIN32_FIND_DATA Win32Data;
	TCHAR szFindPathFile[MAX_PATH];
	PathCombine(szFindPathFile,lpszDirectoryPathFile,_T("*.*"));
	HANDLE hFind = FindFirstFile(szFindPathFile,&Win32Data);
	if (hFind == INVALID_HANDLE_VALUE)
		return 0;

	DWORD dwFileSize = 0;

	do 
	{
		if (_tcsicmp(Win32Data.cFileName,_T(".")) == 0 ||
			_tcsicmp(Win32Data.cFileName,_T("..")) == 0)
			continue;

		TCHAR szFileName[MAX_PATH];
		PathCombine(szFileName,lpszDirectoryPathFile,Win32Data.cFileName);

		if (Win32Data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			dwFileSize += GetFileSizeFromDirectory(szFileName);
		else
			dwFileSize += GetFileSize(szFileName);

	} while (FindNextFile(hFind,&Win32Data));

	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	return dwFileSize;
}

DWORD CLzma::GetFileSize(LPCTSTR lpszPathFile)
{
	DWORD dwFileSize = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	do 
	{
		hFile = CreateFile(lpszPathFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			break;

		dwFileSize = ::GetFileSize(hFile,0);
	} while (FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return dwFileSize;
}