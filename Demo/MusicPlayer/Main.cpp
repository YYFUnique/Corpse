#include "stdafx.h"
#include "resource.h"
#include "PlayerMain.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CPlayerMain* pPlayerMain = new CPlayerMain();
	if( pPlayerMain == NULL ) return 0;
	pPlayerMain->Create(NULL, _T("·ÉÒÝÎÞÏÞ--²¥·ÅÆ÷"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG, 0, 0, 884, 652);
	pPlayerMain->CenterWindow();
	pPlayerMain->ShowWindow(true);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}