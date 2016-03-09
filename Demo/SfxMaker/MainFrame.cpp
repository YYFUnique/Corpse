#include "stdafx.h"
#include "resource.h"
#include "SfxMaker.h"

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

		CSfxMaker* pSfxMaker = new CSfxMaker();
		if (pSfxMaker == NULL) 
			break;

		pSfxMaker->Create(NULL, _T("自解压制作工具"), UI_WNDSTYLE_FRAME, NULL, 0, 0, 800, 600);
		pSfxMaker->CenterWindow();
		pSfxMaker->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}