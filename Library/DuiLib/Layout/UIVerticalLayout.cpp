#include "stdafx.h"
#include "UIVerticalLayout.h"

namespace DuiLib
{
	CVerticalLayoutUI::CVerticalLayoutUI() : m_iSepHeight(0), m_uButtonState(0), m_bImmMode(false),m_bLastResult(FALSE)
	{
		ptLastMouse.x = ptLastMouse.y = 0;
		::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
	}

	LPCTSTR CVerticalLayoutUI::GetClass() const
	{
		return _T("VerticalLayoutUI");
	}

	LPVOID CVerticalLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_VERTICALLAYOUT) == 0 ) return static_cast<CVerticalLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CVerticalLayoutUI::GetControlFlags() const
	{
		if( IsEnabled() && m_iSepHeight != 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void CVerticalLayoutUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;
		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		if( m_items.GetSize() == 0) {
			ProcessScrollBar(rc, 0, 0);
			return;
		}
		//////////////////////////////////////////////////////////////////////////
		///added by gechunping on 2014_04_02 for process border
		if(m_rcBorderSize.left > 0 || m_rcBorderSize.top > 0 || m_rcBorderSize.right > 0 || m_rcBorderSize.bottom > 0){
			rc.left += m_rcBorderSize.left;
			rc.top += m_rcBorderSize.top;
			rc.right -= m_rcBorderSize.right;
			rc.bottom -= m_rcBorderSize.bottom;
		}
		else if(m_nBorderSize>0){
			rc.left += m_nBorderSize;
			rc.top +=m_nBorderSize;
			rc.right -= m_nBorderSize;
			rc.bottom -= m_nBorderSize;
		}
		//////////////////////////////////////////////////////////////////////////
		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cy == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		int cxNeeded = 0;
		if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			iPosY -= m_pVerticalScrollBar->GetScrollPos();
		}
		int iPosX = rc.left;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		}
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cy -= rcPadding.top;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if( sz.cy == 0 ) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				//////////////////////////////////////////////////////////////////////////
				///corrected by gechunping  on 2014_3_27 
				///deleted origin 
				// 				if( iAdjustable == nAdjustables ) {
				// 					sz.cy = MAX(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				// 				} 
				///corrected by gechunping  on 2014_3_27
				//////////////////////////////////////////////////////////////////////////

				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = pControl->GetFixedWidth();
			if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if( sz.cx < 0 ) sz.cx = 0;
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			int tmp = sz.cx + rcPadding.left + rcPadding.right;
			cxNeeded = (tmp > cxNeeded) ? tmp: cxNeeded;
			
			//////////////////////////////////////////////////////////////////////////
			///corrected by gechunping  on 2014_3_27
			///origin///  RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy +  rcPadding.bottom };
			///corrected by gechunping  on 2014_3_27
			//////////////////////////////////////////////////////////////////////////
			pControl->SetPos(rcCtrl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void CVerticalLayoutUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
			RECT rcSeparator = GetThumbRect(true);
			CRenderEngine::DrawColor(hDC, rcSeparator, 0xAA000000);
		}
	}

	void CVerticalLayoutUI::SetSepHeight(int iHeight)
	{
		m_iSepHeight = iHeight;
	}

	int CVerticalLayoutUI::GetSepHeight() const
	{
		return m_iSepHeight;
	}

	void CVerticalLayoutUI::SetSepImmMode(bool bImmediately)
	{
		if( m_bImmMode == bImmediately ) return;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
			m_pManager->RemovePostPaint(this);
		}

		m_bImmMode = bImmediately;
	}

	bool CVerticalLayoutUI::IsSepImmMode() const
	{
		return m_bImmMode;
	}

	void CVerticalLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("sepheight")) == 0 ) SetSepHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcscmp(pstrValue, _T("true")) == 0);
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CVerticalLayoutUI::DoEvent(TEventUI& event)
	{
		if( m_iSepHeight != 0 ) {
			if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
			{
				RECT rcSeparator = GetThumbRect(false);
				if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
					m_uButtonState |= UISTATE_CAPTURED;
					ptLastMouse = event.ptMouse;
					m_rcNewPos = m_rcItem;
					m_rcFixedPos = m_rcNewPos;
					if( !m_bImmMode && m_pManager ) m_pManager->AddPostPaint(this);
					return;
				}
			}
			if( event.Type == UIEVENT_BUTTONUP )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					m_uButtonState &= ~UISTATE_CAPTURED;
					m_rcItem = m_rcNewPos;
					if( !m_bImmMode && m_pManager ) {
						m_pManager->RemovePostPaint(this);

						do 
						{
							//新版本的拖动方式，以前旧的方式不生效了
							CContainerUI* pContainer = (CContainerUI*)GetParent();
							int nIndex = pContainer->GetItemIndex(this);

							if (nIndex ==0 || nIndex == pContainer->GetCount() -1)
								break;

							CContainerUI* pPrev = (CContainerUI*)pContainer->GetItemAt(nIndex -1);
							CContainerUI* pNext = (CContainerUI*)pContainer->GetItemAt(nIndex+1);
							if (pPrev->GetInterface(DUI_CTR_CONTAINER) == NULL)
								break;
							if (pNext->GetInterface(DUI_CTR_CONTAINER) == NULL)
								break;
							RECT rcPos1 = pPrev->GetPos();
							RECT rcPos2 = pNext->GetPos();

							rcPos1.bottom = m_rcItem.top;
							rcPos2.top		 = m_rcItem.bottom;

							int nFixedHeightPrev = rcPos1.bottom - rcPos1.top;
							int nFixedHeightNext = rcPos2.bottom - rcPos2.top;
							if (nFixedHeightPrev < pPrev->GetMinHeight() || nFixedHeightNext < pNext->GetMinHeight())
								break;

							pPrev->SetFixedHeight(nFixedHeightPrev);
							//pNext->SetFixedHeight(nFixedHeightNext);

						} while (FALSE);
					}
					NeedParentUpdate();
					return;
				}
			}
			if( event.Type == UIEVENT_MOUSEMOVE )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					LONG cy = event.ptMouse.y - ptLastMouse.y;
					ptLastMouse = event.ptMouse;
					RECT rc = m_bLastResult ? m_rcNewPos : m_rcFixedPos;
					
					rc.top += cy;
					rc.bottom += cy;

					CDuiRect rcInvalidate = GetThumbRect(true);

					BOOL bSuccess = FALSE;
					CContainerUI* pPrev = NULL;
					CContainerUI* pNext = NULL;
					int nFixedHeightPrev = 0, nFixedHeightNext = 0;
					do 
					{
						//新版本的拖动方式，以前旧的方式不生效了
						CContainerUI* pContainer = (CContainerUI*)GetParent();
						int nIndex = pContainer->GetItemIndex(this);

						if (nIndex ==0 || nIndex == pContainer->GetCount() -1)
							break;

						pPrev = (CContainerUI*)pContainer->GetItemAt(nIndex -1);
						pNext = (CContainerUI*)pContainer->GetItemAt(nIndex+1);

						if (pPrev->GetInterface(DUI_CTR_CONTAINER) == NULL)
							break;
						if (pNext->GetInterface(DUI_CTR_CONTAINER) == NULL)
							break;

						RECT rcPos1 = pPrev->GetPos();
						RECT rcPos2 = pNext->GetPos();

						rcPos1.bottom = rc.top;
						rcPos2.top		 = rc.bottom;

						nFixedHeightPrev = rcPos1.bottom - rcPos1.top;
						nFixedHeightNext = rcPos2.bottom - rcPos2.top;
						if (nFixedHeightPrev < pPrev->GetMinHeight() || nFixedHeightNext < pNext->GetMinHeight())
							break;

						if (nFixedHeightPrev > pPrev->GetMaxHeight() || nFixedHeightNext > pNext->GetMaxHeight())
							break;

						bSuccess = TRUE;
					} while (FALSE);

					m_bLastResult = bSuccess;
					if (m_bLastResult != FALSE)
						m_rcNewPos = rc;
					else
						m_rcFixedPos = rc;

					if( m_bImmMode ) {
						if (m_bLastResult == FALSE)
							return;

						m_rcItem = m_rcNewPos;
						
						pPrev->SetFixedHeight(nFixedHeightPrev);
						//pNext->SetFixedHeight(nFixedHeightNext);
					
						NeedParentUpdate();
					}
					else {
						rcInvalidate.Join(GetThumbRect(true));
						rcInvalidate.Join(GetThumbRect(false));
						if( m_pManager ) m_pManager->Invalidate(rcInvalidate);
					}
					return;
				}
			}
			if( event.Type == UIEVENT_SETCURSOR )
			{
				RECT rcSeparator = GetThumbRect(false);
				if( IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
					return;
				}
			}
		}
		CContainerUI::DoEvent(event);
	}

	RECT CVerticalLayoutUI::GetThumbRect(bool bUseNew) const
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
			if( m_iSepHeight >= 0 ) 
				return CDuiRect(m_rcNewPos.left, MAX(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top), 
				m_rcNewPos.right, m_rcNewPos.bottom);
			else 
				return CDuiRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.right, 
				MIN(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom));
		}
		else {
			if( m_iSepHeight >= 0 ) 
				return CDuiRect(m_rcItem.left, MAX(m_rcItem.bottom - m_iSepHeight, m_rcItem.top), m_rcItem.right, 
				m_rcItem.bottom);
			else 
				return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.right, 
				MIN(m_rcItem.top - m_iSepHeight, m_rcItem.bottom));

		}
	}
}
