#pragma once

class CWnd
{
public:
	CWnd();
	~CWnd();
public:
	LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0);
protected:
	HWND m_hWnd;
};