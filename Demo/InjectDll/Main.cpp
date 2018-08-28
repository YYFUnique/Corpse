#include "stdafx.h"
#include "InjectDll.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	HANDLE hMultex = NULL;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CInjectDll* pInjectDll = new CInjectDll();
		if (pInjectDll == NULL) 
			break;

		pInjectDll->Create(NULL, _T("远程注入工具"), UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
		pInjectDll->CenterWindow();
		pInjectDll->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}