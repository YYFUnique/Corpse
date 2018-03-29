#include "StdAfx.h"
#include "libRpc.h"
HINSTANCE ghModule = NULL;

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch( dwReason ) 
	{
	case DLL_PROCESS_ATTACH:
		ghModule = hModule;
		::DisableThreadLibraryCalls((HMODULE)hModule);
#ifndef _WIN64
		InitRpcModule();
#endif
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		::DisableThreadLibraryCalls((HMODULE)hModule);
#ifndef _WIN64
		UninitRpcModule();
#endif
		break;
	}
	return TRUE;
}