#include "stdafx.h"
#include "QRTool.h"

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

		CQRTool* pQRTools = new CQRTool();
		if (pQRTools == NULL) 
			break;

		pQRTools->Create(NULL, _T("二维码制作工具"), UI_WNDSTYLE_DIALOG, 0, 800, 600);
		pQRTools->CenterWindow();
		pQRTools->ShowWindow(true);
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