#include "stdafx.h"
#include "Login.h"

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

		CLogin* pLogin = new CLogin();
		if (pLogin == NULL) 
			break;

		pLogin->Create(NULL, _T("安全桌面登录窗口"), UI_WNDSTYLE_DIALOG, WS_EX_TOPMOST, 0, 0, 400, 300);
		pLogin->CenterWindow();
		pLogin->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	}while(FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}