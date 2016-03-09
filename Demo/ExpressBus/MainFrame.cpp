#include "StdAfx.h"
/*#include "Resource.h"*/
#include "ExpressBus.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR , int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	HRESULT hRet = ::CoInitialize(NULL);
	if (FAILED(hRet)) 
		return FALSE;

	CExpressBus* pExpressBus = new CExpressBus();
	if (pExpressBus == NULL) 
		return FALSE;

	pExpressBus->Create(NULL, _T("飞逸无限--直通车"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG, 0, 0, 884, 652);
	pExpressBus->CenterWindow();
	pExpressBus->ShowWindow(true);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return TRUE;
}