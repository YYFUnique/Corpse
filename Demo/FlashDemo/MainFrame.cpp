#include "stdafx.h"
#include "FlashDemo.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR , int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	do 
	{
		HRESULT hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CFlashDemo* pFlashDemo = new CFlashDemo();
		if (pFlashDemo == NULL) 
			break;

		pFlashDemo->Create(NULL, _T("FlashDemo"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 800, 600);
		pFlashDemo->CenterWindow();
		pFlashDemo->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		::CoUninitialize();
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}