// libDLNA.cpp : 定义 DLL 应用程序的入口点。
//

#include "DMC.h"
#include "libDLNA.h"

CDMC * g_pDMC = NULL;


static BOOL CALLBACK APlayerEnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwCurrentProcessId = ::GetCurrentProcessId();
	DWORD dwWindowProcessId = 0;
	::GetWindowThreadProcessId(hwnd, &dwWindowProcessId);

	if(dwWindowProcessId == dwCurrentProcessId)
	{
		WCHAR szClassName[MAX_PATH];
		::GetClassNameW(hwnd, szClassName, MAX_PATH);
		if( _wcsicmp(szClassName, L"APlayer_NotifyWindowClass") == 0 ||
			_wcsicmp(szClassName, L"APlayer3_NotifyWindowClass") == 0 )
		{
			HWND * phNotifyWnd = (HWND *)lParam;
			*phNotifyWnd = hwnd;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL IsAPlayerLoaded(void)
{
	BOOL bResult = FALSE;

	// Enum aplayer notify window
	HWND hAPlayerNotifyWnd = NULL;
	::EnumWindows(APlayerEnumWindowsProc, (LPARAM)&hAPlayerNotifyWnd);

	// Get result
	if(hAPlayerNotifyWnd != NULL)
		bResult = TRUE;
	return bResult;
}


LIBDLNA_API BOOL APlayerDlna_Initialize(FileIO* pIO, LPDLNA_EVENT_ROUTINE pfnEventCallBack, LPVOID pCallBackParameter)
{
	if(!IsAPlayerLoaded())
		return FALSE;

	BOOL bResult = FALSE;
	if(g_pDMC == NULL)
	{
		g_pDMC = new CDMC();
		if(g_pDMC != NULL)
			bResult = g_pDMC->DMC_Initialize(pIO, pfnEventCallBack, pCallBackParameter);
	}
    return bResult;
}


LIBDLNA_API BOOL APlayerDlna_UnInitialize()
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
	{
		bResult = g_pDMC->DMC_UnInitialize();
		delete g_pDMC;
		g_pDMC = NULL;
	}
	return bResult;
}


LIBDLNA_API BOOL APlayerDlna_Open(LPCTSTR lpszUrl, HANDLE * phSession)
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
		bResult = g_pDMC->DMC_Open(lpszUrl, phSession);
	return bResult;
}


LIBDLNA_API BOOL APlayerDlna_Close(HANDLE hSession)
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
		bResult = g_pDMC->DMC_Close(hSession);
	return bResult;
}


LIBDLNA_API	BOOL APlayerDlna_SelectDevice(LPCTSTR lpszDeviceID)
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
		bResult = g_pDMC->DMC_SelectDevice(lpszDeviceID);
	return bResult;
}


LIBDLNA_API BOOL APlayerDlna_Play()
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
		bResult = g_pDMC->DMC_Play();
	return bResult;
}


LIBDLNA_API BOOL APlayerDlna_Pause()
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
		bResult = g_pDMC->DMC_Pause();
	return bResult;
}


LIBDLNA_API BOOL APlayerDlna_Stop()
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
		bResult = g_pDMC->DMC_Stop();
	return bResult;
}


LIBDLNA_API BOOL APlayerDlna_SetPosition(LONG nTimePosition)
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
		bResult = g_pDMC->DMC_SetPosition(nTimePosition);
	return bResult;
}


LIBDLNA_API BOOL APlayerDlna_GetPosition(LONG *pnTimePosition)
{
	BOOL bResult = FALSE;
	if(g_pDMC != NULL)
		bResult = g_pDMC->DMC_GetPosition(pnTimePosition);
	return bResult;
}

