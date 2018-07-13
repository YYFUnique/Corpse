#include "stdafx.h"
#include "SDKInterface.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{	 
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;

	do 
	{
		hRet = ::OleInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CSDKInterface* pSDKInterface = new CSDKInterface();
		if (pSDKInterface == NULL) 
			break;

		pSDKInterface->Create(NULL, _T("µçÄÔ¹Ü¼Ò"), UI_WNDSTYLE_FRAME, 0, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT);
		pSDKInterface->CenterWindow();
		pSDKInterface->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::OleUninitialize();

	return bSuccess;
}