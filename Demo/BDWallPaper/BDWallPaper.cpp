#include "stdafx.h"
#include "resource.h"
#include "MainWnd.h"

static LPBYTE resource_zip_buffer_ = NULL;

//#define USE_EMBEDED_RESOURCE

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	
	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CMainWnd* pFrame = new CMainWnd();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("�ٶȱ�ֽ"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 884, 652);
	pFrame->CenterWindow();

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}