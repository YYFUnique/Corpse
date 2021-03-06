#include "stdafx.h"
HINSTANCE g_hInstance;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	g_hInstance = (HINSTANCE) hModule;
	switch( dwReason ) 
	{
		case DLL_PROCESS_ATTACH:
				::DisableThreadLibraryCalls((HMODULE)hModule);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
				::DisableThreadLibraryCalls((HMODULE)hModule);
	}
	return TRUE;
}
