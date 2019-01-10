#include "StdAfx.h"
#include "Wnd.h"

CWnd::CWnd()
	:m_hWnd(NULL)
{

}

CWnd::~CWnd()
{

}

LRESULT CWnd::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (IsWindow(m_hWnd))
		return ::SendMessage(m_hWnd, uMsg, wParam, lParam);

	return FALSE;
}