
#include <windows.h>

// 保存模块句柄，释放DLL时需要用到
static HMODULE gLibModule = 0;

// 用于保存时钟窗口原始处理过程,DLL卸载时应该还原
static LONG_PTR gOldWndProc = 0;

// 查找时钟窗口句柄
static HWND FindClockWindow()
{
	HWND h = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
	if (IsWindow(h))
	{
		h = FindWindowEx(h, 0, TEXT("TrayNotifyWnd"), nullptr);
		if (IsWindow(h))
		{
			return FindWindowEx(h, 0, TEXT("TrayClockWClass"), nullptr);
		}
	}
	return 0;
}

static DWORD WINAPI FreeSelf(LPVOID param)
{
	FreeLibraryAndExitThread(gLibModule, EXIT_SUCCESS);
}

static void RestoreWndProc()
{
	if (gOldWndProc != 0)
		SetWindowLongPtr(FindClockWindow(), GWLP_WNDPROC, gOldWndProc);
}

// 新的窗口处理过程,核心工作都在这里
LRESULT CALLBACK ClockWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 为了能收到鼠标左击右击的消息，必须让Windows以为我们"命中"了工作区
	if (uMsg == WM_NCHITTEST)
		return HTCLIENT;

	// 处理鼠标左击事件
	if (uMsg == WM_LBUTTONUP)
	{
		ShellExecute(0, TEXT("open"), TEXT("http://blog.csdn.net/aqtata"), nullptr, nullptr, SW_SHOW);
		return 0;
	}

	// 处理鼠标右击事件，这里我们卸载DLL(自身)
	if (uMsg == WM_RBUTTONUP)
	{
		RestoreWndProc();
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeSelf, nullptr, 0, nullptr));
		return 0;
	}

	return WNDPROC(gOldWndProc)(hWnd, uMsg, wParam, lParam);
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
			gOldWndProc = GetWindowLongPtr(hClock, GWLP_WNDPROC);
			if (gOldWndProc != 0)
			{
				SetWindowLongPtr(hClock, GWLP_WNDPROC, (LONG_PTR)&ClockWndProc);
			}
		}
		break;
	}

	case DLL_PROCESS_DETACH:
	{
		RestoreWndProc();
		break;
	}
	}
	return TRUE;
}