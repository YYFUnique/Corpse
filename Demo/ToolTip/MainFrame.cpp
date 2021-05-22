#include "StdAfx.h"
#include "ToolTip.h"

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

		CToolTip* pToolTip = new CToolTip();
		if (pToolTip == NULL) 
			break;

		pToolTip->Create(NULL, _T("桌面图标查找工具"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 800, 600);
		pToolTip->CenterWindow();
		pToolTip->ShowWindow(true);

		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}