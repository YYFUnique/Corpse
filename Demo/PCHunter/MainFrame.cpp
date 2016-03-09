#include "stdafx.h"
#include "PCHunter.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	CDuiString strTip;
	do 
	{
		hRet = ::OleInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CPCHunter* pPCHunter = new CPCHunter();
		if (pPCHunter == NULL) 
			break;

		pPCHunter->Create(NULL, _T("µçÄÔ¹Ü¼Ò"), UI_WNDSTYLE_FRAME, 0, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT);
		pPCHunter->CenterWindow();
		pPCHunter->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();
		delete pPCHunter;

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::OleUninitialize();

	return bSuccess;
}