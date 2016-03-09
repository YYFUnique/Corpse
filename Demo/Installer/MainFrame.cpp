#include "stdafx.h"
#include "resource.h"
#include "Installer.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CInstaller* pInstaller = new CInstaller();
		if (pInstaller == NULL) 
			break;

		pInstaller->Create(NULL, _T("自解压安装程序"), UI_WNDSTYLE_FRAME, NULL, 0, 0, 800, 600);
		pInstaller->CenterWindow();
		pInstaller->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}