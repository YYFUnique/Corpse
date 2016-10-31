#include "StdAfx.h"
#include "OsInfo.h"

BOOL OsIsWow64Process()
{
	static BOOL bWow64Process=-1;

	if (bWow64Process==TRUE || bWow64Process==FALSE)
		return bWow64Process;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);

	HMODULE hModule=GetModuleHandle(_T("Kernel32.dll"));

	if (hModule==NULL)
		return FALSE;

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hModule,"IsWow64Process");
	if (fnIsWow64Process==NULL)
		return FALSE;//win2000

	bWow64Process=FALSE;
	if (fnIsWow64Process(GetCurrentProcess(),&bWow64Process)==FALSE)
		return FALSE;

	return bWow64Process;
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