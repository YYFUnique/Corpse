#include "stdafx.h"
#include "CaptureScreen.h"

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

		CCaptureScreen* pCaptureScreen = new CCaptureScreen();
		if (pCaptureScreen == NULL) 
			break;

		pCaptureScreen->Create(NULL, _T("·É·É½ØÍ¼"), UI_WNDSTYLE_DIALOG, WS_EX_TOPMOST, 0, 0, 400, 300);
		pCaptureScreen->CenterWindow();
		pCaptureScreen->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	}while(FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}