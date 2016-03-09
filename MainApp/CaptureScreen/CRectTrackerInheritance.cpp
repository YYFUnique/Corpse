#include "stdafx.h"
#include "CRectTrackerInheritance.h"

CRectTrackerInheritance::CRectTrackerInheritance()
{
	m_hCursors[DIRECTE_WNES] = ::LoadCursor(NULL, IDC_SIZENWSE);//西北-东南方向箭头 ATL_MAKEINTRESOURCE(AFX_IDC_TRACKNWSE));
	m_hCursors[DIRECTE_ENWS] = ::LoadCursor(NULL, IDC_SIZENESW);//东北-西南 (AFX_IDC_TRACKNESW));
	m_hCursors[DIRECTE_ESWN] = m_hCursors[DIRECTE_WNES];
	m_hCursors[DIRECTE_WSEN] = m_hCursors[DIRECTE_ENWS];
	m_hCursors[DIRECTE_NS] = ::LoadCursor(NULL, IDC_SIZENS);//南北 (AFX_IDC_TRACKNS));
	m_hCursors[DIRECTE_EW] = ::LoadCursor(NULL, IDC_SIZEWE);//东西 (AFX_IDC_TRACKWE));
	m_hCursors[DIRECTE_SN] = m_hCursors[DIRECTE_NS];
	m_hCursors[DIRECTE_WE] = m_hCursors[DIRECTE_EW];
	m_hCursors[DIRECTE_ALL] = ::LoadCursor(NULL, IDC_SIZEALL);//四方 (AFX_IDC_TRACK4WAY));
	m_hCursors[9] = ::LoadCursor(NULL, ATL_MAKEINTRESOURCE(AFX_IDC_MOVE4WAY));
}

CRectTrackerInheritance::~CRectTrackerInheritance()
{
}

void CRectTrackerInheritance::SetRect(LPRECT lpRect)
{
	m_rect.SetRect(lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
}

void CRectTrackerInheritance::SetCursor(CWnd* pWnd, UINT nHitTest)
{
	if (nHitTest != HTCLIENT)
		return;

	CPoint point;
	GetCursorPos(&point);
	pWnd->ScreenToClient(&point);

	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
		return;

	nHandle = NormalizeHit(nHandle);

	if (nHandle == hitMiddle && !m_rect.PtInRect(point))
	{
		if (m_nStyle & hatchedBorder)
			nHandle = (TrackerHit)9;
	}

	ENSURE(nHandle < _countof(m_hCursors));
	::SetCursor(m_hCursors[nHandle]);
}

void CRectTrackerInheritance::SetCursor(UINT nHitTest)
{
	if (nHitTest >=0 && nHitTest <= 9)
		::SetCursor(m_hCursors[nHitTest]);
}

int CRectTrackerInheritance::GetHitTestHandle(CPoint point)
{
	return HitTestHandles(point);
}

void CRectTrackerInheritance::DrawTrackerRect( LPCRECT lpRect, CWnd* pWndClipTo,CDC* pDC, CWnd* pWnd )
{	    
	//更新窗口,消除TRACK时的虚线框
	//((CScreenSpyDlg *)pWnd)->PaintWindow();
	//((CScreenSpyDlg *)pWnd)->SendMessage(WM_MOUSEMOVE);		
}

