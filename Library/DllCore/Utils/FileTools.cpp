#include "StdAfx.h"
#include "FileTools.h"
#include "ErrorInfo.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "version.lib")

BOOL SHDeleteDirectory(LPCTSTR lpszPathName)
{
	SHFILEOPSTRUCT    shFileOp = {0};

	TCHAR szPathName[MAX_PATH+1];
	ZeroMemory(szPathName, _countof(szPathName));

	//由于pFrom需要双终结符，故采用此方式
	_tcsncpy_s(szPathName, _countof(szPathName), lpszPathName, _tcslen(lpszPathName));

	shFileOp.wFunc    = FO_DELETE;
	shFileOp.pFrom   = szPathName;
	shFileOp.pTo		 = NULL;
	shFileOp.fFlags    = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;

	//如果SHFileOperation返回0表示操作成功
	return SHFileOperation(&shFileOp) == 0;
}

// 检查一个文件是否是64位的
BOOL CheckFileIsX64(LPCTSTR lpszFilePath)
{
	BOOL bRet = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	IMAGE_DOS_HEADER ImageDosHeader;
	IMAGE_NT_HEADERS ImageNtHeader;
	ULONGLONG ullFileSize = 0;
	do
	{
		if (PathFileExists(lpszFilePath))
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("无法找到指定的文件"));
			throw 0;
		}

		hFile = CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("打开文件失败!"));
			throw 0;
		}

		DWORD dwFileSizeLow,dwFileSizeHigh;
		dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);
		ullFileSize = (((ULONGLONG)dwFileSizeHigh)<<32) + dwFileSizeLow;

		if (SetFilePointer(hFile, 0, 0, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("检查PE文件DOS头部信息时移动文件指针失败"));
			break;
		}

		DWORD dwNumberOfBytesToRead=0;
		if (ReadFile(hFile, &ImageDosHeader, sizeof(ImageDosHeader), &dwNumberOfBytesToRead, 0)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("读取PE文件DOS头部信息时失败"));
			break;
		}

		if (dwNumberOfBytesToRead < sizeof(IMAGE_DOS_HEADER))
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("无效的PE文件:文件长度不正确!"));
			break;
		}

		if (ImageDosHeader.e_magic != 0x5a4d)//"MZ"
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("无效的PE文件:文件DOS头标志不正确!"));
			break;
		}

		if ((DWORD)ImageDosHeader.e_lfanew > ullFileSize)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("无效的PE文件:PE文件头偏移不正确!"));
			break;
		}

		if (SetFilePointer(hFile, ImageDosHeader.e_lfanew, 0, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("检查PE文件NT头部信息时移动文件指针失败"));
			break;
		}

		if (ReadFile(hFile, &ImageNtHeader, sizeof(ImageNtHeader), &dwNumberOfBytesToRead, 0)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("读取PE文件NT头部信息时失败"));
			break;
		}

		if (dwNumberOfBytesToRead < sizeof(IMAGE_NT_HEADERS))
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("无效的PE文件:文件长度不正确!"));
			break;
		}

		if (memcmp(&ImageNtHeader.Signature,"PE\0\0",4))
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("无效的PE文件:文件NT头标志不正确!"));
			break;
		}

		//如果是x64文件则返回成功
		if (ImageNtHeader.FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) // 如果x64
			bRet = TRUE;

	}while(FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return bRet;
}

CString GetVersionText(DWORD dwVersion)
{
	CString strVersion;
	strVersion.Format(_T("V%d.%d.%d.%d"),dwVersion>>24,(dwVersion>>16)&0xff,(dwVersion>>8)&0xff,dwVersion&0xff);
	return strVersion;
}

DWORD GetFileVersion(LPCTSTR lpszFilePath)
{
	DWORD dwVersion=0x1000000;//1.0.0.0

	//如果文件不存在，则返回默认文件版本
	if (PathFileExists(lpszFilePath) == FALSE)
		return dwVersion;
	
	DWORD dwHandle = 0;
	VS_FIXEDFILEINFO* pVersion = NULL;
	DWORD dwVersionSize = GetFileVersionInfoSize(lpszFilePath, &dwHandle);

	if (dwVersionSize > 0)
	{
		LPVOID lpData = LocalAlloc(LMEM_ZEROINIT, dwVersionSize);

		if (GetFileVersionInfo(lpszFilePath, dwHandle, dwVersionSize, lpData))
		{
			UINT uLen = 0;
			if (VerQueryValue(lpData, _T("\\"), (void **)&pVersion, &uLen))
			{
				dwVersion=((pVersion->dwFileVersionMS&0xff0000)<<8) | ((pVersion->dwFileVersionMS&0xff)<<16) |
					((pVersion->dwFileVersionLS&0xff0000)>>8) | (pVersion->dwFileVersionLS&0xff) ;
			}			
		}
		LocalFree(lpData);
	}

	return dwVersion;
}

CString GetModuleFileVersion()
{
	TCHAR szModuleAppPath[MAX_PATH];
	GetModuleFileName(0 , szModuleAppPath , _countof(szModuleAppPath));
	DWORD dwVersion = GetFileVersion(szModuleAppPath);

	return GetVersionText(dwVersion);
}

BOOL GetCommonAppDataFilePath(LPCTSTR lpszFileDirectory,LPCTSTR lpszFileTitle,LPTSTR lpszFilePath)
{
	lpszFilePath[0]=0;

	HRESULT hResult=SHGetFolderPath(0,CSIDL_COMMON_APPDATA,0,SHGFP_TYPE_CURRENT,lpszFilePath);
	if (hResult != S_OK)
	{
		if (SHGetFolderPath(0,CSIDL_COMMON_APPDATA,0,SHGFP_TYPE_DEFAULT,lpszFilePath)!=S_OK)
		{
			GetWindowsDirectory(lpszFilePath,MAX_PATH);
			PathStripToRoot(lpszFilePath);
			PathAppend(lpszFilePath,_T("Documents and Settings\\All Users\\Application Data"));
		}
	}
	PathAppend(lpszFilePath,_T("Corpse"));

	if (PathIsDirectory(lpszFilePath) == FALSE)
		SHCreateDirectoryEx(0,lpszFilePath,0);

	PathAppend(lpszFilePath,lpszFileDirectory);
	if (PathIsDirectory(lpszFilePath) == FALSE)
		SHCreateDirectoryEx(0,lpszFilePath,0);

	return PathAppend(lpszFilePath,lpszFileTitle);
}

CString GetProgramDataFilePath(LPCTSTR lpszDirName,LPCTSTR lpszName,LPCTSTR lpszVersion /* = NULL*/)
{
	ASSERT(lpszDirName && lstrlen(lpszDirName));
	ASSERT(lpszName && lstrlen(lpszName));

	TCHAR szLogPath[MAX_PATH];
	GetCommonAppDataFilePath(lpszDirName,_T(""),szLogPath);
	if (PathIsDirectory(szLogPath)==FALSE)
	{
		if (CreateDirectory(szLogPath,0)==FALSE)
			::PathRemoveFileSpec(szLogPath);
	}

	//如果第一个字符是"."，则跳过
	LPCTSTR lpszExtendName = lpszName;
	if (lpszName[0] == _T('.'))
		lpszExtendName = lpszName+1;
	
	//如果版本号前面未添加"-"，则添加一个"-"
	CString strVersion(lpszVersion);
	if (strVersion.IsEmpty() == FALSE)
		strVersion.Insert(0,_T("-"));

	SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);
	CString szFileName;
	szFileName.Format(_T("%s(%02d-%02d-%02d)%s.%s"),GetLogModuleFileName() ,
								SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,strVersion, lpszExtendName);

	if (szFileName.GetLength()+_tcslen(szLogPath)+2>MAX_PATH)
		::GetSystemDirectory(szLogPath,MAX_PATH);

	::PathAppend(szLogPath,szFileName);
	return szLogPath;
}
