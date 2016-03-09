#include "stdafx.h"
#include "Toolbar.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{

// 	int sum = 0;
// 	Lua= lua_open();
// 	luaopen_base(L);
// 	luaL_openlibs(L);
// 	luaL_loadfile(L, "add.lua");
// 	lua_pcall(L, 0, LUA_MULTRET, 0);
// 	/*sum = luaAdd(10, 15);*/
// 
// 	return TRUE;

// 
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	HANDLE hMultex = NULL;
	do 
	{
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;

		CToolbar* pToolbar = new CToolbar();
		if (pToolbar == NULL) 
			break;

		pToolbar->Create(NULL, _T("×ÀÃæ¹¤¾ßÀ¸"), UI_WNDSTYLE_DIALOG,WS_EX_TOOLWINDOW,0, 0, 800, 600);
		pToolbar->CenterWindow();
		pToolbar->ShowWindow(true);
		CPaintManagerUI::MessageLoop();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}