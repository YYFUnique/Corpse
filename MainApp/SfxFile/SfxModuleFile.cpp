#include "StdAfx.h"
#include "Resource.h"
#include "SfxModuleFile.h"
#include "Lzma/MakeSfxFile.h"

#define		WINDOW_CLASS_NAME			_T("SfxMaker")
#define		WINDOW_TITLE_NAME				_T("自解压程序工具")

BOOL CSfxModuleFile::OnInitWindow(HINSTANCE hInstance, UINT nWidth, UINT nHeight)
{
	WNDCLASS WinClass;

	WinClass.style         = CS_HREDRAW | CS_VREDRAW;
	WinClass.lpfnWndProc   = WndProc; 
	WinClass.cbClsExtra    = 0;
	WinClass.cbWndExtra    = 0;
	WinClass.hInstance     = hInstance;
	WinClass.hIcon         = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MAINFRAME));
	WinClass.hCursor       = LoadCursor(0, IDC_ARROW);
	WinClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WinClass.lpszMenuName  = 0;
	WinClass.lpszClassName = WINDOW_CLASS_NAME;

	BOOL bSuccess = FALSE;

	do 
	{
		if (RegisterClass(&WinClass) == FALSE)
		{
			::MessageBox(0, _T("注册窗口类失败了！"), _T("提示"), MB_OK);
			break;
		}

		HWND hWnd = ::CreateWindow(WINDOW_CLASS_NAME,WINDOW_TITLE_NAME, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,nWidth,nHeight,NULL, NULL, hInstance, NULL); 
		if (hWnd == NULL)
		{
			::MessageBox(0, _T("创建Windows窗口失败了！"), _T("提示"), MB_OK);
			break;
		}

		::ShowWindow(hWnd, SW_SHOW);
		::UpdateWindow(hWnd);

		bSuccess = TRUE;
	} while (FALSE);	

	return bSuccess;
}

LRESULT CALLBACK CSfxModuleFile::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CREATE:
			OnCreate();
			break;
		case WM_DESTROY:
			PostQuitMessage(IDOK);
			break;
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

BOOL CSfxModuleFile::OnCreate()
{
	CMakeSfxFile MakeSfx;
	MakeSfx.ReleaseData();
	return TRUE;
}

void CSfxModuleFile::MessageLoop()
{
	MSG Msg;
	while(GetMessage(&Msg,NULL,0,0))
	{
		TranslateMessage(&Msg); 
		DispatchMessage(&Msg); 
	}
}