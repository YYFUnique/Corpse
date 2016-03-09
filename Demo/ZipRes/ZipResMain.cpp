#include "StdAfx.h"
#include "ZipRes.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CZipRes* pZipRes = new CZipRes();
	if( pZipRes == NULL ) return 0;
	pZipRes->Create(NULL, _T("°²×°³ÌÐò"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 800, 600);
	pZipRes->CenterWindow();
	pZipRes->ShowWindow(true);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}
