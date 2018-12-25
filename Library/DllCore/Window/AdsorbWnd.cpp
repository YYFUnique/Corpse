#include "StdAfx.h"
#include "AdsorbWnd.h"
#include "DllCore/Log/LogHelper.h"

AdsorbWnd AdsorbWnd::m_sAdsorbWnd;

AdsorbWnd* AdsorbWnd::GetInstance()
{
	return &m_sAdsorbWnd;
}

AdsorbWnd::AdsorbWnd()
	: m_pWndProc(NULL)
	, m_nDistance(DOCK_DISTANCE)
	, m_DockType(DOCK_TYPE_RIGHT)
	, m_bDocked(FALSE)
{
	ZeroMemory(&m_PtLastMouse, sizeof(POINT));
	m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
}

AdsorbWnd::~AdsorbWnd()
{

}

void AdsorbWnd::Initialize(HWND hWnd, UINT nDistance /*= DOCK_DISTANCE*/, DOCK_TYPE DockType /*= DOCK_TYPE_RIGHT*/)
{
	m_pWndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	if (m_pWndProc == NULL)
		return;

	m_nDistance = nDistance;
	m_DockType = DockType;

	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (ULONG_PTR)&AdsorbWnd::__WndProc);
}

void AdsorbWnd::SetScreenSize(LPARAM lParam)
{
	m_nScreenWidth = LOWORD(lParam);
	// 扣除任务栏高度
	m_nScreenHeight = HIWORD(lParam);
}

UINT_PTR AdsorbWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	AdsorbWnd *pAdsorbWnd = GetInstance();

	return pAdsorbWnd->AdsorbWndProc(hWnd, uMsg, wParam, lParam);
}

UINT AdsorbWnd::AdsorbWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_MOVING:
			{
				if (m_bDocked == FALSE)
					break;

				POINT pt;
				GetCursorPos(&pt);
				BOOL bAttatch = TRUE;
				RECT* rcPos = (RECT*)lParam;
				POINT ptClientPos = {rcPos->left, rcPos->top};

				switch (m_DockType)
				{
				case DOCK_TYPE_RIGHT:
					if (m_PtLastMouse.x - pt.x > m_nDistance)
						bAttatch = FALSE;
					break;
				case DOCK_TYPE_LEFT:
					if (pt.x - m_PtLastMouse.x > m_nDistance)
						bAttatch = FALSE;
					break;
				case DOCK_TYPE_TOP:
					if (pt.y - m_PtLastMouse.y > m_nDistance)
						bAttatch = FALSE;
					break;
				case DOCK_TYPE_BOTTOM:
					if (m_PtLastMouse.y - pt.y > m_nDistance)
						bAttatch = FALSE;
					break;
				case DOCK_TYPE_ALL: 
					if (m_PtLastMouse.x - pt.x > m_nDistance /*right*/ || pt.x - m_PtLastMouse.x > m_nDistance /*left*/ || 
						pt.y - m_PtLastMouse.y > m_nDistance /*top*/ || m_PtLastMouse.y - pt.y > m_nDistance /*bottom*/)
							bAttatch = FALSE;
					break;
				}
		
				if (bAttatch == FALSE)
				{
					m_bDocked = FALSE;
					UpdateSubWndPos(hWnd, ptClientPos);
				}
			}
			break;
		case WM_WINDOWPOSCHANGING:
			{
				WINDOWPOS* prcPos = (WINDOWPOS*)lParam;
				if (prcPos->flags & SWP_NOMOVE)
					break;

				if (::IsWindowVisible(hWnd) == FALSE)
					break;

				RECT rcMain;
				GetWindowRect(hWnd, &rcMain);
				// 避免在窗口显示之前存在误差
				if (abs(rcMain.left - prcPos->x) > m_nDistance)
					break;

				POINT ptClientPos = {prcPos->x, prcPos->y};

				switch (m_DockType)
				{
					case DOCK_TYPE_LEFT:
						if (rcMain.left <= m_nDistance)
						{
							m_bDocked = TRUE;
							ptClientPos.x = 0;
						}
						break;
					case DOCK_TYPE_TOP:
						if (rcMain.top <= m_nDistance)
						{
							m_bDocked = TRUE;
							ptClientPos.y = 0;
						}
						break;
					case DOCK_TYPE_RIGHT:
						if (m_nScreenWidth - rcMain.right <= m_nDistance)
						{
							m_bDocked = TRUE;
							ptClientPos.x = m_nScreenWidth - prcPos->cx;
						}
						break;
					case DOCK_TYPE_BOTTOM:
						if (m_nScreenHeight - rcMain.bottom <= m_nDistance)
						{
							m_bDocked = TRUE;
							ptClientPos.y = m_nScreenHeight - prcPos->cy;
						}
						break;
					case DOCK_TYPE_ALL:
						if (rcMain.left <= m_nDistance) { // left
							m_bDocked = TRUE;
							ptClientPos.x = 0;
						} else if (rcMain.top <= m_nDistance) {	// top
							m_bDocked = TRUE;
							ptClientPos.y = 0;
						} else if (m_nScreenWidth - rcMain.right <= m_nDistance) {	//right
							m_bDocked = TRUE;
							ptClientPos.x = m_nScreenWidth - prcPos->cx;
						} else if (m_nScreenHeight - rcMain.bottom <= m_nDistance) { // bottom
							m_bDocked = TRUE;
							ptClientPos.y = m_nScreenHeight - prcPos->cy;
						}
						break;
				}

				// 让系统自动使用更新后的值
				prcPos->x = ptClientPos.x;
				prcPos->y = ptClientPos.y;
				if (m_bDocked)
					UpdateSubWndPos(hWnd, ptClientPos);
				else
					GetCursorPos(&m_PtLastMouse);
			}
			break;
		case WM_DISPLAYCHANGE:
				SetScreenSize(lParam);	
			break;
		case WM_NCLBUTTONDOWN:
				m_PtLastMouse.x = GET_X_LPARAM(lParam);
				m_PtLastMouse.y = GET_Y_LPARAM(lParam);
			break;
	}

	if (m_pWndProc != NULL)
		return m_pWndProc(hWnd, uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void AdsorbWnd::UpdateSubWndPos(HWND hWnd, POINT pt)
{
	if (::IsWindow(hWnd))
		SetWindowPos(hWnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}