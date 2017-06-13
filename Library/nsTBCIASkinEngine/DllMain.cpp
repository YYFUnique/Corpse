#include "stdafx.h"

HINSTANCE g_hInstance;

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	g_hInstance = (HINSTANCE) hInst;

	return TRUE;
}
