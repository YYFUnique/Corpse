#include "StdAfx.h"
#include "TrayClock.h"

// 保存模块句柄，释放DLL时需要用到
HMODULE gLibModule = 0;

// 用于保存时钟窗口原始处理过程,DLL卸载时应该还原
/*static LONG_PTR gOldWndProc = 0;*/

CTrayClock* pTrayClock = NULL;

// 查找时钟窗口句柄
static HWND FindClockWindow()
{
	HWND hTrayWnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	if (IsWindow(hTrayWnd))
	{
		HWND hNotifyWnd = FindWindowEx(hTrayWnd, 0, _T("TrayNotifyWnd"), NULL);
		if (IsWindow(hNotifyWnd))
			return FindWindowEx(hNotifyWnd, 0, _T("TrayClockWClass"), NULL);
	}
	return NULL;
}

static DWORD WINAPI FreeSelf(LPVOID param)
{
	FreeLibraryAndExitThread(gLibModule, EXIT_SUCCESS);
}

//static void RestoreWndProc()
//{
//	if (gOldWndProc != 0)
//		SetWindowLongPtr(FindClockWindow(), GWLP_WNDPROC, gOldWndProc);
//}

void RefreshRebar(HWND hwndRebar)
{
	if(hwndRebar)
	{
		HWND hwnd;

		InvalidateRect(hwndRebar, NULL, TRUE);
		hwnd = GetWindow(hwndRebar, GW_CHILD);
		while(hwnd)
		{
			InvalidateRect(hwnd, NULL, TRUE);
			hwnd = GetWindow(hwnd, GW_HWNDNEXT);
		}
	}
}

void RefreshTaskbar(HWND hWndClock)
{
	HWND hWndTaskbar, hWndRebar, hWndTray/*, hwndStartButton*/;

	hWndTray = GetParent(hWndClock);
	hWndTaskbar = GetParent(hWndTray);
	hWndRebar = FindWindowEx(hWndTaskbar, NULL, _T("ReBarWindow32"), NULL);
	//hwndStartButton = FindWindowEx(hwndTaskbar, NULL, _T("Button"), NULL);

	//InvalidateRect(hwndStartButton, NULL, TRUE);
	InvalidateRect(hWndTray, NULL, TRUE);
	PostMessage(hWndTray, WM_SIZE, SIZE_RESTORED, 0);
 
	// 使用该行代码导致桌面应用闪烁，后面完善时移除该部分代码
	//RefreshRebar(hwndRebar);
	//PostMessage(hwndRebar, WM_SIZE, SIZE_RESTORED, 0);

	InvalidateRect(hWndTaskbar, NULL, TRUE);
	PostMessage(hWndTaskbar, WM_SIZE, SIZE_RESTORED, 0);
}

void FreeRemoteLibrary(HWND hWnd)
{
	RefreshTaskbar(hWnd);
	pTrayClock->OnFinalMessage(hWnd);
	CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeSelf, NULL, 0, NULL));
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			gLibModule = hModule;
			HWND hClock = FindClockWindow();
			if (IsWindow(hClock))
			{
				gLibModule = hModule;
				pTrayClock = new CTrayClock;
				pTrayClock->Init(hClock);
				RefreshTaskbar(hClock);
			}
			break;
		}

	case DLL_PROCESS_DETACH:
		{
			break;
		}
	}
	return TRUE;
}