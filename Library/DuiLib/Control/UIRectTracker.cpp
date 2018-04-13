#include "StdAfx.h"
#include "UIRectTracker.h"

namespace DuiLib
{
	typedef struct tagAFX_HANDLEINFO
	{
		size_t nOffsetX;    // offset within RECT for X coordinate
		size_t nOffsetY;    // offset within RECT for Y coordinate
		int nCenterX;       // adjust X by Width()/2 * this number
		int nCenterY;       // adjust Y by Height()/2 * this number
		int nHandleX;       // adjust X by handle size * this number
		int nHandleY;       // adjust Y by handle size * this number
		int nInvertX;       // handle converts to this when X inverted
		int nInvertY;       // handle converts to this when Y inverted
	}AFX_HANDLEINFO;

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

	typedef struct tagAFX_RECTINFO
	{
		size_t nOffsetAcross;   // offset of opposite point (ie. left->right)
		int nSignAcross;        // sign relative to that point (ie. add/subtract)
	}AFX_RECTINFO;

	// this array is indexed by the offset of the RECT member / sizeof(int)
	const AFX_RECTINFO _afxRectInfo[] =
	{
		{ offsetof(RECT, right), +1 },
		{ offsetof(RECT, bottom), +1 },
		{ offsetof(RECT, left), -1 },
		{ offsetof(RECT, top), -1 },
	};

	#define AFX_CX_BORDER   1
	#define AFX_CY_BORDER   1

	HCURSOR CRectTrackerUI::m_hCursors[10];

	CRectTrackerUI::CRectTrackerUI()
	{
		m_nBorderSize = 1;
		m_dwBorderColor = 0xFF0AADFE;
	}

	LPCTSTR CRectTrackerUI::GetClass() const
	{
		return _T("RectTrackerUI");
	}

	LPVOID CRectTrackerUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_RECTTRACKER) == 0)	return static_cast<CRectTrackerUI*>(this);
		else 
			return CLabelUI::GetInterface(pstrName);	
	}

	UINT CRectTrackerUI::GetControlFlags() const
	{
		return UIFLAG_SETCURSOR;
	}

	void CRectTrackerUI::DoInit()
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

		m_nStyle = 0;
		m_nHandleSize = 5*m_nBorderSize;
		m_sizeMin.cy = m_sizeMin.cx = m_nHandleSize;
		m_rectLast.Empty();
		m_sizeLast.cx = m_sizeLast.cy = -1;
		m_bFinalErase =  FALSE;

		m_nStyle = solidLine|resizeOutside;
	}

	void CRectTrackerUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETCURSOR){
			SetCursor(m_pManager->GetPaintWindow(),HTCLIENT);
			return;
		}
		
		CLabelUI::DoEvent(event);
	}

	void CRectTrackerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CRectTrackerUI::PaintBorder(HDC hDC)
	{
		CLabelUI::PaintBorder(hDC);

		UINT mask = GetHandleMask();
		CDuiRect rcBox;
		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1<<i))
			{
				GetHandleRect((TrackerHit)i, &rcBox);
				CRenderEngine::DrawColor(hDC, rcBox, m_dwBorderColor);
			}
		}
	}

	int CRectTrackerUI::HitTest(POINT ptCursor) const
	{
		TrackerHit hitResult = hitNothing;

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

	BOOL CRectTrackerUI::SetCursor(HWND hWnd, UINT nHitTest) 
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

	int CRectTrackerUI::HitTestHandles(POINT pt) const
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

	UINT CRectTrackerUI::GetHandleMask() const
	{
		UINT mask = 0x0F;   // always have 4 corner handles
		int size = m_nHandleSize*3;
		if (abs(m_rect.GetWidth()) - size > 4)
			mask |= 0x50;
		if (abs(m_rect.GetHeight()) - size > 4)
			mask |= 0xA0;
		return mask;
	}

	void CRectTrackerUI::AdjustRect(int nHandle, LPRECT)
	{
		if (nHandle == hitMiddle)
			return;

		// convert the handle into locations within m_rect
		int *px, *py;
		GetModifyPointers(nHandle, &px, &py, NULL, NULL);

		// enforce minimum width
		int nNewWidth = m_rect.GetWidth();
		int nAbsWidth = m_bAllowInvert ? abs(nNewWidth) : nNewWidth;
		if (px != NULL && nAbsWidth < m_sizeMin.cx)
		{
			nNewWidth = nAbsWidth != 0 ? nNewWidth / nAbsWidth : 1;
			ptrdiff_t iRectInfo = (int*)px - (int*)&m_rect;
			ASSERT(0 <= iRectInfo && iRectInfo < _countof(_afxRectInfo));
			const AFX_RECTINFO* pRectInfo = &_afxRectInfo[iRectInfo];
			*px = *(int*)((BYTE*)&m_rect + pRectInfo->nOffsetAcross) +
				nNewWidth * m_sizeMin.cx * -pRectInfo->nSignAcross;
		}

		// enforce minimum height
		int nNewHeight = m_rect.GetHeight();
		int nAbsHeight = m_bAllowInvert ? abs(nNewHeight) : nNewHeight;
		if (py != NULL && nAbsHeight < m_sizeMin.cy)
		{
			nNewHeight = nAbsHeight != 0 ? nNewHeight / nAbsHeight : 1;
			ptrdiff_t iRectInfo = (int*)py - (int*)&m_rect;
			ASSERT(0 <= iRectInfo && iRectInfo < _countof(_afxRectInfo));
			const AFX_RECTINFO* pRectInfo = &_afxRectInfo[iRectInfo];
			*py = *(int*)((BYTE*)&m_rect + pRectInfo->nOffsetAcross) +
				nNewHeight * m_sizeMin.cy * -pRectInfo->nSignAcross;
		}
	}

	void CRectTrackerUI::GetTrueRect(LPRECT lpTrueRect) const
	{
		ASSERT(lpTrueRect);

		CDuiRect rect = GetPos();
		rect.Normalize();
		int nInflateBy = 0;
		if ((m_nStyle & (resizeOutside|hatchedBorder)) != 0)
			nInflateBy += GetHandleSize() - 1;
		if ((m_nStyle & (solidLine|dottedLine)) != 0)
			++nInflateBy;
		rect.Inflate(nInflateBy, nInflateBy);
		*lpTrueRect = rect;
	}

	int CRectTrackerUI::GetHandleSize(LPCRECT lpRect) const
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

	void CRectTrackerUI::GetHandleRect(int nHandle, LPRECT pHandleRect) const
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
			rectT.Inflate(size/2-1, size/2-1);

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

	int CRectTrackerUI::NormalizeHit(int nHandle) const
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

	void CRectTrackerUI::DrawTrackerRect(LPCRECT lpRect, HDC hDC)
	{
		CDuiRect rect = *lpRect;
		rect.Normalize();
		CSize size(0, 0);

		if (!m_bFinalErase)
		{
			// otherwise, size depends on the style
			if (m_nStyle & hatchedBorder)
			{
				size.cx = size.cy = max(1, GetHandleSize(&rect)-1);
				rect.Inflate(size.cx,size.cy);
			}
			else
			{
				size.cx = AFX_CX_BORDER;
				size.cy = AFX_CY_BORDER;
			}

			if(m_nStyle & (dottedLine|solidLine))
			{
				int offset=GetHandleSize(&rect)/2;
				rect.Inflate(offset,offset);
			}
		}

		if (m_bFinalErase || !m_bErase)
			NeedParentUpdate();

		m_rectLast = rect;
		m_sizeLast = size;
	}

	BOOL CRectTrackerUI::Track(HWND hWnd, POINT pt, BOOL bAllowInvert)
	{
		// perform hit testing on the handles
		int nHandle = HitTestHandles(pt);
		if (nHandle < 0)
		{
			// didn't hit a handle, so just return FALSE
			return FALSE;
		}

		// otherwise, call helper function to do the tracking
		m_bAllowInvert = bAllowInvert;
		return TrackHandle(nHandle, hWnd);
	}

	BOOL CRectTrackerUI::TrackHandle(int nHandle, HWND hWnd)
	{
		ASSERT(nHandle >= 0);
		ASSERT(nHandle <= 8);   // handle 8 is inside the rect

		// don't handle if capture already set
		if (::GetCapture() != NULL)
			return FALSE;

		ASSERT(!m_bFinalErase);

		// save original width & height in pixels
		int nWidth = m_rect.GetWidth();
		int nHeight = m_rect.GetHeight();

		// set capture to the window which received this message
		SetCapture(hWnd);
		ASSERT(hWnd == GetCapture());
		UpdateWindow(hWnd);
		CDuiRect rectSave = m_rect;

		// convert cursor position to client co-ordinates
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hWnd,&pt);

		// find out what x/y coords we are supposed to modify
		int *px, *py;
		int xDiff, yDiff;
		GetModifyPointers(nHandle, &px, &py, &xDiff, &yDiff);
		xDiff = pt.x - xDiff;
		yDiff = pt.y - yDiff;

		// get DC for drawing
		HDC hDrawDC = GetDC(hWnd);

		CDuiRect rectOld;
		BOOL bMoved = FALSE;
		
		// get messages until capture lost or cancelled/accepted
		for (;;)
		{
			MSG msg;
			//VERIFY(::GetMessage(&msg, NULL, 0, 0));
			::GetMessage(&msg, NULL, 0, 0);
			if (GetCapture() != hWnd)
				break;

			//DispatchMessage(&msg);
			switch (msg.message)
			{
				// handle movement/accept messages
			case WM_LBUTTONUP:
			case WM_MOUSEMOVE:
				rectOld = m_rect;
				// handle resize cases (and part of move)
				if (px != NULL)
					*px = GET_X_LPARAM(msg.lParam) - xDiff;
				if (py != NULL)
					*py = GET_Y_LPARAM(msg.lParam) - yDiff;

				// handle move case
				if (nHandle == hitMiddle)
				{
					m_rect.right = m_rect.left + nWidth;
					m_rect.bottom = m_rect.top + nHeight;
				}
				// allow caller to adjust the rectangle if necessary
				AdjustRect(nHandle, &m_rect);

				// only redraw and callback if the rect actually changed!
				m_bFinalErase = (msg.message == WM_LBUTTONUP);
				if (!rectOld.EqualRect(&m_rect) || m_bFinalErase)
				{
					if (bMoved)
					{
						m_bErase = TRUE;
						DrawTrackerRect(&rectOld, hDrawDC);
					}

					CDuiRect rcItem = m_rect;
					rcItem.Normalize();
					SetPos(rcItem);

					if (m_pManager)
						m_pManager->SendNotify(this, DUI_MSGTYPE_POSCHANGED);

					OnChangedRect(rectOld);
					if (msg.message != WM_LBUTTONUP)
						bMoved = TRUE;
				}
				if (m_bFinalErase)
					goto ExitLoop;

				if (!rectOld.EqualRect(&m_rect))
				{
					m_bErase = FALSE;
					DrawTrackerRect(&m_rect, hDrawDC);
				}
				break;

				// handle cancel messages
			case WM_KEYDOWN:
				if (msg.wParam != VK_ESCAPE)
					break;
			case WM_RBUTTONDOWN:
				if (bMoved)
				{
					m_bErase = m_bFinalErase = TRUE;
					DrawTrackerRect(&m_rect, hDrawDC);
				}
				m_rect = rectSave;
				goto ExitLoop;

				// just dispatch rest of the messages
			default:
				DispatchMessage(&msg);
				break;
			}
		}

ExitLoop:
		ReleaseDC(hWnd, hDrawDC);
		ReleaseCapture();

		// restore rect in case bMoved is still FALSE
		if (!bMoved)
			m_rect = rectSave;
		m_bFinalErase = FALSE;
		m_bErase = FALSE;

		// return TRUE only if rect has changed
		return !rectSave.EqualRect(&m_rect);
	}

	BOOL CRectTrackerUI::TrackRubberBand(HWND hWnd, POINT pt, BOOL bAllowInvert)
	{
		// simply call helper function to track from bottom right handle
		m_bAllowInvert = bAllowInvert;
		m_rect.SetRect(pt, pt);
		return TrackHandle(hitBottomRight, hWnd);
	}

	void CRectTrackerUI::GetModifyPointers(int nHandle, int** ppx, int** ppy, int* px, int* py)
	{
		ASSERT(nHandle >= 0);
		ASSERT(nHandle <= 8);

		if (nHandle == hitMiddle)
			nHandle = hitTopLeft;   // same as hitting top-left

		*ppx = NULL;
		*ppy = NULL;

		// fill in the part of the rect that this handle modifies
		//  (Note: handles that map to themselves along a given axis when that
		//   axis is inverted don't modify the value on that axis)

		const AFX_HANDLEINFO* pHandleInfo = &_afxHandleInfo[nHandle];
		if (pHandleInfo->nInvertX != nHandle)
		{
			*ppx = (int*)((BYTE*)&m_rect + pHandleInfo->nOffsetX);
			if (px != NULL)
				*px = **ppx;
		}
		else
		{
			// middle handle on X axis
			if (px != NULL)
				*px = m_rect.left + abs(m_rect.GetWidth()) / 2;
		}
		if (pHandleInfo->nInvertY != nHandle)
		{
			*ppy = (int*)((BYTE*)&m_rect + pHandleInfo->nOffsetY);
			if (py != NULL)
				*py = **ppy;
		}
		else
		{
			// middle handle on Y axis
			if (py != NULL)
				*py = m_rect.top + abs(m_rect.GetHeight()) / 2;
		}
	}

	void CRectTrackerUI::OnChangedRect(const RECT& rectOld)
	{

	}
}
