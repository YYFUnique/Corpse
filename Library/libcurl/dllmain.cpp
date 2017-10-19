#include <Windows.h>
#include "libcurl/curl/curl.h"
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch( dwReason ) {
   case DLL_PROCESS_ATTACH:
	   curl_global_init(CURL_GLOBAL_WIN32);
	   ::DisableThreadLibraryCalls((HMODULE)hModule);
	   break;
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
	   ::DisableThreadLibraryCalls((HMODULE)hModule);
	   break;
	}
	return TRUE;
}