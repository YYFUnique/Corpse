#pragma once

class IMenuHandler
{
public:
	virtual LRESULT OnCMD(HWND hWnd, WPARAM wParam, LPARAM lParam) = 0;
};