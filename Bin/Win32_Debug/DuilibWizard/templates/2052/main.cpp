[!if PRE_COMPILED_HEADER]
#include "stdafx.h"
[!endif]
#include "[!output PROJECT_NAME].h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	do 
	{
[!if USE_OLE_INITIALIZECOM]
		hRet = ::OleInitialize(NULL);
[!else]
		hRet = ::CoInitialize(NULL);
[!endif]
		if (FAILED(hRet)) 
			break;

		C[!output SAFE_PROJECT_IDENTIFIER_NAME]* p[!output SAFE_PROJECT_IDENTIFIER_NAME] = new C[!output SAFE_PROJECT_IDENTIFIER_NAME]();
		if (p[!output SAFE_PROJECT_IDENTIFIER_NAME] == NULL) 
			break;

		p[!output SAFE_PROJECT_IDENTIFIER_NAME]->Create(NULL, _T("[!output DIALOG_TITLE]"), UI_WNDSTYLE_FRAME, WS_EX_TOOLWINDOW, 0, 0, 800, 600);
		p[!output SAFE_PROJECT_IDENTIFIER_NAME]->CenterWindow();
		p[!output SAFE_PROJECT_IDENTIFIER_NAME]->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
[!if USE_OLE_INITIALIZECOM]
		hRet = ::OleUninitialize();
[!else]
		::CoUninitialize();
[!endif]

	return bSuccess;
}