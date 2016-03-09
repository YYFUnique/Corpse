#include "stdafx.h"
#include "LoadPE.h"

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

		CLoadPE* pLoadPE = new CLoadPE();
		if (pLoadPE == NULL) 
			break;

		pLoadPE->Create(NULL, _T("PE±à¼­Æ÷"), UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
		pLoadPE->CenterWindow();
		pLoadPE->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}