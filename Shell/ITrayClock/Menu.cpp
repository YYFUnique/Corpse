#include "StdAfx.h"
#include "Menu.h"

CMenu::CMenu()
	: m_hPopupMenu(NULL)
	, m_pMenuHandler(NULL)
{

}

CMenu::~CMenu()
{
	if (m_hPopupMenu != NULL)
	{
		::DestroyMenu(m_hPopupMenu);
	}
}

void CMenu::DestroyMenu()
{
	::DestroyMenu(m_hPopupMenu);
	m_hPopupMenu = NULL;
}

BOOL CMenu::CreatePopupMenu()
{
	// 如果之前存在菜单句柄，继续使用
	if (m_hPopupMenu != NULL)
		return TRUE;

	m_hPopupMenu = ::CreatePopupMenu();
	if (m_hPopupMenu == NULL)
		return FALSE;
	return TRUE;
}

BOOL CMenu::ShowPopupMenu(HWND hWnd, UINT nFlags, POINT* pClickPt)
{
	if (m_hPopupMenu == NULL)
		return FALSE;

	SetForegroundWindow(hWnd);
	return TrackPopupMenu(m_hPopupMenu, nFlags, pClickPt->x, pClickPt->y, 0, hWnd, NULL);
}

void CMenu::SetMenuHandler(IMenuHandler* pMenuHandler)
{
	m_pMenuHandler = pMenuHandler;
}

LRESULT CMenu::OnCmd(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (m_pMenuHandler != NULL)
		return	m_pMenuHandler->OnCMD(hWnd, wParam, lParam);
	return TRUE;
}

BOOL CMenu::InsertMenu(UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCTSTR lpszNewItem)
{
	return ::InsertMenu(m_hPopupMenu, uPosition, uFlags, uIDNewItem, lpszNewItem);
}