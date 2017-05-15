#include "stdafx.h"
#include "Postman.h"

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

		CPostman* pPostman = new CPostman();
		if (pPostman == NULL) 
			break;

		pPostman->Create(NULL, _T("接口测试工具"), UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
		pPostman->CenterWindow();
		pPostman->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}