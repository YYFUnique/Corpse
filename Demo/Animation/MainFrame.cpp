#include "stdafx.h"
#include "UIAnimation.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_OK;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CAnimationUI* pAnimation = new CAnimationUI();
		if (pAnimation == NULL) 
			break;

		pAnimation->Create(NULL, _T("µçÄÔ¹Ü¼Ò"), UI_WNDSTYLE_DIALOG, 0, 0, 800, 600);
		pAnimation->CenterWindow();
		pAnimation->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		delete pAnimation;
		CPaintManagerUI::Term();
		
		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}