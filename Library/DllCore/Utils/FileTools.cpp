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

	//����pFrom��Ҫ˫�ս�����ʲ��ô˷�ʽ
	_tcsncpy_s(szPathName, _countof(szPathName), lpszPathName, _tcslen(lpszPathName));

	shFileOp.wFunc    = FO_DELETE;
	shFileOp.pFrom   = szPathName;
	shFileOp.pTo		 = NULL;
	shFileOp.fFlags    = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;

	//���SHFileOperation����0��ʾ�����ɹ�
	return SHFileOperation(&shFileOp) == 0;
}

// ���һ���ļ��Ƿ���64λ��
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
			SetErrorInfo(SYSTEM_ERROR, 0, _T("�޷��ҵ�ָ�����ļ�"));
			throw 0;
		}

		hFile = CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("���ļ�ʧ��!"));
			throw 0;
		}

		DWORD dwFileSizeLow,dwFileSizeHigh;
		dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);
		ullFileSize = (((ULONGLONG)dwFileSizeHigh)<<32) + dwFileSizeLow;

		if (SetFilePointer(hFile, 0, 0, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("���PE�ļ�DOSͷ����Ϣʱ�ƶ��ļ�ָ��ʧ��"));
			break;
		}

		DWORD dwNumberOfBytesToRead=0;
		if (ReadFile(hFile, &ImageDosHeader, sizeof(ImageDosHeader), &dwNumberOfBytesToRead, 0)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ȡPE�ļ�DOSͷ����Ϣʱʧ��"));
			break;
		}

		if (dwNumberOfBytesToRead < sizeof(IMAGE_DOS_HEADER))
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��Ч��PE�ļ�:�ļ����Ȳ���ȷ!"));
			break;
		}

		if (ImageDosHeader.e_magic != 0x5a4d)//"MZ"
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��Ч��PE�ļ�:�ļ�DOSͷ��־����ȷ!"));
			break;
		}

		if ((DWORD)ImageDosHeader.e_lfanew > ullFileSize)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��Ч��PE�ļ�:PE�ļ�ͷƫ�Ʋ���ȷ!"));
			break;
		}

		if (SetFilePointer(hFile, ImageDosHeader.e_lfanew, 0, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("���PE�ļ�NTͷ����Ϣʱ�ƶ��ļ�ָ��ʧ��"));
			break;
		}

		if (ReadFile(hFile, &ImageNtHeader, sizeof(ImageNtHeader), &dwNumberOfBytesToRead, 0)==FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ȡPE�ļ�NTͷ����Ϣʱʧ��"));
			break;
		}

		if (dwNumberOfBytesToRead < sizeof(IMAGE_NT_HEADERS))
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��Ч��PE�ļ�:�ļ����Ȳ���ȷ!"));
			break;
		}

		if (memcmp(&ImageNtHeader.Signature,"PE\0\0",4))
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��Ч��PE�ļ�:�ļ�NTͷ��־����ȷ!"));
			break;
		}

		//�����x64�ļ��򷵻سɹ�
		if (ImageNtHeader.FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) // ���x64
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

	//����ļ������ڣ��򷵻�Ĭ���ļ��汾
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

	//�����һ���ַ���"."��������
	LPCTSTR lpszExtendName = lpszName;
	if (lpszName[0] == _T('.'))
		lpszExtendName = lpszName+1;
	
	//����汾��ǰ��δ���"-"�������һ��"-"
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
