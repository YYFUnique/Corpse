#include "StdAfx.h"
#include "ShellDesktop.h"

HMODULE ghModule = NULL;
CShellDesktop* pShellDesktop = NULL;
IFinalCallback* pFinalCallback = NULL;

HWND FindProgman()
{
	return FindWindow(_T("Progman"), _T("Program Manager"));
}

static DWORD WINAPI FreeSelf(LPVOID param)
{
	if (ghModule)
		FreeLibraryAndExitThread(ghModule, EXIT_SUCCESS);

	return TRUE;
}

class CFreeLibrary : public IFinalCallback
{
public:
	void OnFinalCallback(){
		CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeSelf, NULL, 0, NULL));
	}
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				OutputDebugString(_T("DLL_PROCESS_ATTACH"));
				ghModule = hModule;
				HWND hProgman = FindProgman();
				if (IsWindow(hProgman))
				{
					pFinalCallback = new CFreeLibrary;
					pShellDesktop = new CShellDesktop(pFinalCallback);
					pShellDesktop->Init(hProgman);
				}
			}
			break;
		case DLL_PROCESS_DETACH:
		{
			OutputDebugString(_T("DLL_PROCESS_DETACH"));
			ghModule = NULL;
			break;
		}
	}
	return TRUE;
}