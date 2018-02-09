#include "StdAfx.h"
#include "TxPCMain.h"

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

		CTxPCMain* pTxPCMain = new CTxPCMain();
		if (pTxPCMain == NULL) 
			break;

		pTxPCMain->Create(NULL, _T("µçÄÔ¹Ü¼Ò"), UI_WNDSTYLE_DIALOG, 0, 370, 600);
		pTxPCMain->CenterWindow();
		pTxPCMain->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	if (hMultex != NULL)
		CloseHandle(hMultex);

	return bSuccess;
}