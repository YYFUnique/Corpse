#pragma once

class CSfxModuleFile
{
public:
	static BOOL OnInitWindow(HINSTANCE hInstance,UINT nWidth,UINT nHeight);
	static BOOL OnCreate();
	static LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	static void MessageLoop();
};