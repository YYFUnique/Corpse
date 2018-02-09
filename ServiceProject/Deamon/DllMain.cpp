#include "StdAfx.h"

HINSTANCE ghModule = NULL;

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
		ghModule = hModule;
		::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	}
	return TRUE;
}