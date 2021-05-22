#include "stdafx.h"
#include "OEMInfoModify.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = CoInitialize(NULL);
	if (FAILED(hRet))
		return 0;

	COEMInfoModify* pOEMInfoModify = new COEMInfoModify();
	if (pOEMInfoModify == NULL) 
		return 0;

	pOEMInfoModify->Create(NULL, _T("OEM设备信息修改器"), UI_WNDSTYLE_DIALOG, 0, 800, 600);
	pOEMInfoModify->CenterWindow();
	pOEMInfoModify->ShowWindow(true);

	CPaintManagerUI::MessageLoop();

	CPaintManagerUI::Term();

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}