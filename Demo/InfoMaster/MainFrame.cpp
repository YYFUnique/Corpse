#include "stdafx.h"
#include "InfoMaster.h"

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

		CInfoMaster* pInfoMaster = new CInfoMaster();
		if (pInfoMaster == NULL) 
			break;

		pInfoMaster->Create(NULL, _T("�����ն˰�ȫ���ù���"), UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
		pInfoMaster->CenterWindow();
		pInfoMaster->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		CPaintManagerUI::Term();
	} while (FALSE);
	
	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return TRUE;
}