#pragma once

class CMenu
{
public:
	CMenu();
	~CMenu();

public:
	void DestroyMenu();
	BOOL CreatePopupMenu();
	void SetMenuHandler(IMenuHandler* pMenuHandler);
	LRESULT OnCmd(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL ShowPopupMenu(HWND hWnd, UINT nFlags, POINT* pClickPt);
	BOOL InsertMenu(UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCTSTR lpszNewItem);
protected:
	HMENU m_hPopupMenu;
	IMenuHandler* m_pMenuHandler;
};