#include <windows.h>
#include "FileDiskMain.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourceZip(_T("skin.zip"));

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

// #ifndef _DEBUG
// 	LsSetUnhandledExceptionFilter(LsUnhandledExceptionFilter);
// #endif

	CFileDiskMain* pFrame = new CFileDiskMain();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("FileDisk--ÐéÄâ´ÅÅÌ¹¤¾ß"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
	pFrame->CenterWindow();
/*	pFrame->ShowWindow(true);*/
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}