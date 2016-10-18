#include "StdAfx.h"
#include "RectTracker.h"

// the struct below is used to determine the qualities of a particular handle
struct AFX_HANDLEINFO
{
	size_t nOffsetX;    // offset within RECT for X coordinate
	size_t nOffsetY;    // offset within RECT for Y coordinate
	int nCenterX;       // adjust X by Width()/2 * this number
	int nCenterY;       // adjust Y by Height()/2 * this number
	int nHandleX;       // adjust X by handle size * this number
	int nHandleY;       // adjust Y by handle size * this number
	int nInvertX;       // handle converts to this when X inverted
	int nInvertY;       // handle converts to this when Y inverted
};

// this array describes all 8 handles (clock-wise)
const AFX_HANDLEINFO _afxHandleInfo[] =
{
	// corner handles (top-left, top-right, bottom-right, bottom-left
	{ offsetof(RECT, left), offsetof(RECT, top),        0, 0,  0,  0, 1, 3 },
	{ offsetof(RECT, right), offsetof(RECT, top),       0, 0, -1,  0, 0, 2 },
	{ offsetof(RECT, right), offsetof(RECT, bottom),    0, 0, -1, -1, 3, 1 },
	{ offsetof(RECT, left), offsetof(RECT, bottom),     0, 0,  0, -1, 2, 0 },

	// side handles (top, right, bottom, left)
	{ offsetof(RECT, left), offsetof(RECT, top),        1, 0,  0,  0, 4, 6 },
	{ offsetof(RECT, right), offsetof(RECT, top),       0, 1, -1,  0, 7, 5 },
	{ offsetof(RECT, left), offsetof(RECT, bottom),     1, 0,  0, -1, 6, 4 },
	{ offsetof(RECT, left), offsetof(RECT, top),        0, 1,  0,  0, 5, 7 }
};

CRectTracker::CRectTracker()
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
	//m_hCursors[DIRECTE_NOTHINE] = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));

	// get default handle size from Windows profile setting
	static const TCHAR szWindows[] = _T("windows");
	static const TCHAR szInplaceBorderWidth[] =
		_T("oleinplaceborderwidth");
	m_nHandleSize = GetProfileInt(szWindows, szInplaceBorderWidth, 4);
}

/*CRectTracker::CRectTracker(LPCRECT lpSrcRect, UINT nStyle)
{
	m_rect.CopyRect(lpSrcRect);
	m_nStyle = nStyle;
}*/

CRectTracker::~CRectTracker()
{

}

int CRectTracker::HitTest(POINT ptCursor) const
{
	TrackerHit hitResult = hitNothing;

	//CRect rectTrue;
	CDuiRect rcTrue;
	GetTrueRect(&rcTrue);
	ASSERT(rcTrue.left <= rcTrue.right);
	ASSERT(rcTrue.top <= rcTrue.bottom);
	if (rcTrue.PtInRect(ptCursor))
	{
		if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
			hitResult = (TrackerHit)HitTestHandles(ptCursor);
		else
			hitResult = hitMiddle;
	}
	return hitResult;
}

void CRectTracker::GetTrueRect(LPRECT lpTrueRect) const
{
	ASSERT(lpTrueRect);

	CDuiRect rect = m_rect;
	rect.Normalize();
	int nInflateBy = 0;
	if ((m_nStyle & (resizeOutside|hatchedBorder)) != 0)
		nInflateBy += GetHandleSize() - 1;
	if ((m_nStyle & (solidLine|dottedLine)) != 0)
		++nInflateBy;
	rect.Inflate(nInflateBy, nInflateBy);
	*lpTrueRect = rect;
}

int CRectTracker::GetHandleSize(LPCRECT lpRect) const
{
	if (lpRect == NULL)
		lpRect = &m_rect;

	int size = m_nHandleSize;
	if (!(m_nStyle & resizeOutside))
	{
		// make sure size is small enough for the size of the rect
		int sizeMax = min(abs(lpRect->right - lpRect->left),
			abs(lpRect->bottom - lpRect->top));
		if (size * 2 > sizeMax)
			size = sizeMax / 2;
	}
	return size;
}

int CRectTracker::HitTestHandles(POINT pt) const
{
	CDuiRect rect;
	UINT mask = GetHandleMask();

	GetTrueRect(&rect);
	if (!rect.PtInRect(pt))
		return hitNothing;  // totally missed

	// see if we hit a handle
	for (int i = 0; i < 8; ++i)
	{
		if (mask & (1<<i))
		{
			GetHandleRect((TrackerHit)i, &rect);
			if (rect.PtInRect(pt))
				return (TrackerHit)i;
		}
	}

	// last of all, check for non-hit outside of object, between resize handles
	if ((m_nStyle & hatchedBorder) == 0)
	{
		CDuiRect rect = m_rect;
		rect.Normalize();
		if ((m_nStyle & dottedLine|solidLine) != 0)
			rect.Inflate(+1, +1);
		if (!rect.PtInRect(pt))
			return hitNothing;  // must have been between resize handles
	}
	return hitMiddle;   // no handle hit, but hit object (or object border)
}

void CRectTracker::GetHandleRect(int nHandle, LPRECT pHandleRect) const
{
	ASSERT(nHandle < 8);

	// get normalized rectangle of the tracker
	CDuiRect rectT = m_rect;
	rectT.Normalize();
	if ((m_nStyle & (solidLine|dottedLine)) != 0)
		rectT.Inflate(+1, +1);

	// since the rectangle itself was normalized, we also have to invert the
	//  resize handles.
	nHandle = NormalizeHit(nHandle);

	// handle case of resize handles outside the tracker
	int size = GetHandleSize();
	if (m_nStyle & resizeOutside)
		rectT.Inflate(size-1, size-1);

	// calculate position of the resize handle
	int nWidth = rectT.GetWidth();
	int nHeight = rectT.GetHeight();
	CDuiRect rect;
	const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
	rect.left = *(int*)((BYTE*)&rectT + pHandleInfo->nOffsetX);
	rect.top = *(int*)((BYTE*)&rectT + pHandleInfo->nOffsetY);
	rect.left += size * pHandleInfo->nHandleX;
	rect.top += size * pHandleInfo->nHandleY;
	rect.left += pHandleInfo->nCenterX * (nWidth - size) / 2;
	rect.top += pHandleInfo->nCenterY * (nHeight - size) / 2;
	rect.right = rect.left + size;
	rect.bottom = rect.top + size;

	*pHandleRect = rect;
}

UINT CRectTracker::GetHandleMask() const
{
	UINT mask = 0x0F;   // always have 4 corner handles
	int size = m_nHandleSize*3;
	if (abs(m_rect.GetWidth()) - size > 4)
		mask |= 0x50;
	if (abs(m_rect.GetHeight()) - size > 4)
		mask |= 0xA0;
	return mask;
}

BOOL CRectTracker::SetCursor(HWND hWnd, UINT nHitTest) 
{
	// trackers should only be in client area
	if (nHitTest != HTCLIENT)
		return FALSE;

	// convert cursor position to client co-ordinates
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd,&pt);

	// do hittest and normalize hit
	int nHandle = HitTestHandles(pt);
	if (nHandle < 0)
		return FALSE;

	// need to normalize the hittest such that we get proper cursors
	nHandle = NormalizeHit(nHandle);

	// handle special case of hitting area between handles
	//  (logically the same -- handled as a move -- but different cursor)
	if (nHandle == hitMiddle && !m_rect.PtInRect(pt))
	{
		// only for trackers with hatchedBorder (ie. in-place resizing)
		if (m_nStyle & hatchedBorder)
			nHandle = (TrackerHit)9;
	}

	::SetCursor(m_hCursors[nHandle]);
	return TRUE;
}

void CRectTracker::SetRect(LPRECT lpRect)
{
	m_rect.CopyRect(lpRect);
}

int CRectTracker::NormalizeHit(int nHandle) const
{
	if (nHandle == hitMiddle || nHandle == hitNothing)
		return nHandle;

	const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
	if (m_rect.GetWidth() < 0)
	{
		nHandle = (TrackerHit)pHandleInfo->nInvertX;
		pHandleInfo = &_afxHandleInfo[nHandle];
	}
	if (m_rect.GetHeight() < 0)
		nHandle = (TrackerHit)pHandleInfo->nInvertY;
	return nHandle;
}

//int CRectTracker::NormalizeHit(int nHandle) const
//{
//	ENSURE(nHandle <= 8 && nHandle >= -1);
//	if (nHandle == hitMiddle || nHandle == hitNothing)
//		return nHandle;
//	ENSURE(0 <= nHandle && nHandle < _countof(_afxHandleInfo));
//	const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
//	if (m_rect.Width() < 0)
//	{
//		nHandle = (TrackerHit)pHandleInfo->nInvertX;
//		ENSURE(0 <= nHandle && nHandle < _countof(_afxHandleInfo));
//		pHandleInfo = &_afxHandleInfo[nHandle];
//	}
//	if (m_rect.Height() < 0)
//		nHandle = (TrackerHit)pHandleInfo->nInvertY;
//	return nHandle;
//}