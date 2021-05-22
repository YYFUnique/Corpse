#pragma once

class CMenuHandler : public IMenuHandler
{	
public:
	CMenuHandler();
	~CMenuHandler();
public:
	virtual LRESULT OnCMD(HWND hWnd, WPARAM wParam, LPARAM lParam);
};