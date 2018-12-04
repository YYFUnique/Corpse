#include "StdAfx.h"
#include "OsInfo.h"
#include "ErrorInfo.h"

#define WINDOWS_COMPUTER_PLANTFORM   ((bWow64) ? (_T("x64")) :(_T("x86")))

typedef BOOL (WINAPI *FN_IsWow64Process)(HANDLE hProcess,PBOOL Wow64Process);
typedef void   (WINAPI *FN_GetNativeSystemInfo)(LPSYSTEM_INFO);

BOOL OsIsWow64Process()
{
	static BOOL bWow64Process = -1;

	if (bWow64Process==TRUE || bWow64Process==FALSE)
		return bWow64Process;

	HMODULE hModule=GetModuleHandle(_T("Kernel32.dll"));

	if (hModule==NULL)
		return FALSE;

	FN_IsWow64Process fnIsWow64Process = (FN_IsWow64Process)GetProcAddress(hModule,"IsWow64Process");
	if (fnIsWow64Process==NULL)
		return FALSE; //win2000

	bWow64Process=FALSE;
	if (fnIsWow64Process(GetCurrentProcess(),&bWow64Process)==FALSE)
		return FALSE;

	return bWow64Process;
}

BOOL Is64BitOS()
{
	BOOL bSuccess = FALSE;
	do 
	{
		HMODULE hModule=GetModuleHandle(_T("Kernel32.dll"));

		if (hModule == NULL)
			break;

		FN_GetNativeSystemInfo fnGetNativeSystemInfo = (FN_GetNativeSystemInfo)GetProcAddress(hModule, "GetNativeSystemInfo");
		if (fnGetNativeSystemInfo==NULL)
			break;

		SYSTEM_INFO SystemInfo;
		fnGetNativeSystemInfo(&SystemInfo);
		if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || 
			SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
			bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL OsIsVistaOrLater() 
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;

	// Initialize the condition mask.

	VER_SET_CONDITION(dwlConditionMask,VER_MAJORVERSION,VER_GREATER_EQUAL);

	// Perform the test.
	return VerifyVersionInfo(&osvi,VER_MAJORVERSION,dwlConditionMask);
}

BOOL OsIsWinXpOrLater()
{
	OSVERSIONINFOEX osvi;
	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;

	// Initialize the condition mask.	
	DWORDLONG dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION,VER_GREATER_EQUAL);

	// Perform the test.
	return VerifyVersionInfo(&osvi,VER_MAJORVERSION | VER_MINORVERSION ,dwlConditionMask);
}

BOOL OsIsWinXpSp2OrLater()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;
	osvi.wServicePackMajor = 2;

	// Initialize the condition mask.
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR,VER_GREATER_EQUAL);

	// Perform the test.
	return VerifyVersionInfo(&osvi,VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR,dwlConditionMask);
}

BOOL OsIsWorkstationWinXpSp2OrLater()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;
	osvi.wServicePackMajor = 2;
	osvi.wProductType = VER_NT_WORKSTATION;

	// Initialize the condition mask.
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_PRODUCT_TYPE,VER_EQUAL);

	// Perform the test.
	return VerifyVersionInfo(&osvi,VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_PRODUCT_TYPE,dwlConditionMask);
}

BOOL OsIsWorkstationVistaOrLater()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.wProductType = VER_NT_WORKSTATION;
	// Initialize the condition mask.

	VER_SET_CONDITION(dwlConditionMask,VER_MAJORVERSION,VER_GREATER_EQUAL);

	// Perform the test.
	return VerifyVersionInfo(&osvi,VER_MAJORVERSION,dwlConditionMask);
}

BOOL OsIsWinXpSp2OrWin2003Sp1Later()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 2;
	osvi.wServicePackMajor = 0;

	// Initialize the condition mask.
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION,VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION,VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR,VER_EQUAL);

	//是否是未打补丁的2003?
	if (VerifyVersionInfo(&osvi,VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR,dwlConditionMask)==TRUE)
		return FALSE;

	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;
	osvi.wServicePackMajor = 2;

	dwlConditionMask = 0;
	// Initialize the condition mask.
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR,VER_GREATER_EQUAL);


	// Perform the test.
	return VerifyVersionInfo(&osvi,VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR,dwlConditionMask);
}

CString GetOsTypeName()
{
	OSVERSIONINFOEX osvi;
	memset(&osvi,0,sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);

	GetVersionEx((LPOSVERSIONINFO)&osvi);

	CString strOsType;
	BOOL bWow64 = OsIsWow64Process();
	if (osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
	{
		if (osvi.dwMajorVersion==4)
			if (osvi.dwMinorVersion==0)
				strOsType=_T("Windows 95");
			else if (osvi.dwMinorVersion==10)
				strOsType=_T("Windows 98");
			else if (osvi.dwMinorVersion==90)
				strOsType=_T("Windows Me");
	}
	else if (osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		if (osvi.dwMajorVersion==4)
		{
			if (osvi.dwMinorVersion==0)
				strOsType=_T("Windows NT 4.0");
		}
		else if (osvi.dwMajorVersion==5)
		{
			if (osvi.dwMinorVersion==0)
				strOsType=_T("Windows 2000");
			else if (osvi.dwMinorVersion==1)
				strOsType.Format(_T("Windows XP(%s)"), WINDOWS_COMPUTER_PLANTFORM);
			else if (osvi.dwMinorVersion==2)
				strOsType.Format(_T("Windows 2003(%s)"), WINDOWS_COMPUTER_PLANTFORM);
		}
		else if (osvi.dwMajorVersion==6)
		{
			if (osvi.dwMinorVersion==0)
				if (osvi.wProductType==VER_NT_WORKSTATION)
					strOsType.Format(_T("Windows Vista(%s)"), WINDOWS_COMPUTER_PLANTFORM);
				else
					strOsType.Format(_T("Windows 2008(%s)"), WINDOWS_COMPUTER_PLANTFORM);
			else if (osvi.dwMinorVersion==1)
				strOsType.Format(_T("Windows 7(%s)"), WINDOWS_COMPUTER_PLANTFORM);
			else if (osvi.dwMinorVersion == 2)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					strOsType.Format(_T("Windows 8(%s)"), WINDOWS_COMPUTER_PLANTFORM);
				else
					strOsType = _T("Windows Server 2012(x64)");
			}
			else if(osvi.dwMinorVersion ==3)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					strOsType.Format(_T("Windows 8.1(%s)"), WINDOWS_COMPUTER_PLANTFORM);
				else
					strOsType = _T("Windows Server 2012 R2(x64)");
			}
		}
		else if (osvi.dwMajorVersion==10)
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					strOsType.Format(_T("Windows 10(%s)"), WINDOWS_COMPUTER_PLANTFORM);
				else
					strOsType = _T("Windows Server 2016(x64)");
			}
		}
	}

	if (strOsType.IsEmpty())
		strOsType.Format(_T("未知操作系统(%d.%d)"),osvi.dwMajorVersion,osvi.dwMinorVersion);

	return strOsType;
}

BOOL OsIsServer()
{
	OSVERSIONINFOEX OsVertionInfo;
	memset(&OsVertionInfo,0,sizeof(OsVertionInfo));
	OsVertionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!GetVersionEx((LPOSVERSIONINFO)&OsVertionInfo))
		return FALSE;

	if (OsVertionInfo.wProductType==VER_NT_WORKSTATION)
		return FALSE;

	return TRUE;
}

BOOL OsIsWin8OrLater()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6;
	osvi.dwMinorVersion = 2;

	// Initialize the condition mask.

	VER_SET_CONDITION(dwlConditionMask,VER_MAJORVERSION,VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask,VER_MINORVERSION,VER_GREATER_EQUAL);
	// Perform the test.
	return VerifyVersionInfo(&osvi,VER_MAJORVERSION|VER_MINORVERSION,dwlConditionMask);
}

CString GetOsSpText()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	GetVersionEx((OSVERSIONINFO*) &osvi);

	return osvi.szCSDVersion;
}

BOOL OsIsVista()
{
	OSVERSIONINFOEX osvi;
	memset(&osvi,0,sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);

	GetVersionEx((LPOSVERSIONINFO)&osvi);

	if (osvi.dwPlatformId==VER_PLATFORM_WIN32_NT)
		if (osvi.dwMajorVersion==6)
			if (osvi.dwMinorVersion==0)
				if (osvi.wProductType==VER_NT_WORKSTATION)
					return TRUE;

	return FALSE;
}

BOOL OsIsWin7()
{
	OSVERSIONINFO VersionInfo;
	ZeroMemory(&VersionInfo,sizeof(OSVERSIONINFO));
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&VersionInfo))
	{
		if ((6 == VersionInfo.dwMajorVersion)&&(1 == VersionInfo.dwMinorVersion))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL GetOsVersion(DWORD& dwMajorVersion, DWORD& dwMinorVersion)
{
	OSVERSIONINFOEX osvi;
	memset(&osvi,0,sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((LPOSVERSIONINFO)&osvi) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("获取系统信息失败"));
		return FALSE;
	}
	dwMajorVersion = osvi.dwMajorVersion;
	dwMinorVersion = osvi.dwMinorVersion;

	return TRUE;
}

CString GetOsInLogicalDrive()
{
	CString strDriver;
	TCHAR szWindowsDirectory[MAX_PATH];
	if (GetSystemWindowsDirectory(szWindowsDirectory,sizeof(szWindowsDirectory)) == 0)
		return _T("");

	if (PathStripToRoot(szWindowsDirectory) == FALSE)
		return _T("");

	strDriver = szWindowsDirectory;
	return strDriver;
}

CString GetSystemDirectory(LPCTSTR lpszFileName)
{
	TCHAR szSystemPath[MAX_PATH];
	GetSystemDirectory(szSystemPath,sizeof(szSystemPath));
	if (lpszFileName != NULL && lpszFileName[0] != NULL)
		PathCombine(szSystemPath,szSystemPath,lpszFileName);

	return szSystemPath;
}

CString GetWindowsDirectory()
{
	TCHAR szWindowsDirectory[MAX_PATH];
	GetSystemWindowsDirectory(szWindowsDirectory,sizeof(szWindowsDirectory));
	return szWindowsDirectory;
}