#include "StdAfx.h"
#include "UISplitLayout.h"

namespace DuiLib
{
	CSplitLayoutUI::CSplitLayoutUI()
		: m_bImmMode(false)
		, m_uButtonState(0)
		, m_bVertical(true)
		, m_iSepWidth(5)
		, m_iSepIndex(-1)
		, m_dwSepLineColor(0xAA000000)
	{

	}

	LPCTSTR CSplitLayoutUI::GetClass() const
	{
		return _T("SplitLayoutUI");
	}

	LPVOID CSplitLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_SPLITLAYOUT) == 0)
			return static_cast<CSplitLayoutUI*>(this);
		else
			return CContainerUI::GetInterface(pstrName);
	}

	UINT CSplitLayoutUI::GetControlFlags() const
	{
		if (IsEnabled()) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void CSplitLayoutUI::SetSepImmMode(bool bImmediately)
	{
		if( m_bImmMode == bImmediately ) return;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
			m_pManager->RemovePostPaint(this);
		}

		m_bImmMode = bImmediately;
	}

	void CSplitLayoutUI::SetVertical(bool bVertical)
	{
		if (m_bVertical == bVertical) return;
		m_bVertical = bVertical;
	}

	void CSplitLayoutUI::SetSepWidth(int nSepWidth)
	{
		if (m_iSepWidth == nSepWidth) return;
		m_iSepWidth = nSepWidth;
		Invalidate();
	}

	void CSplitLayoutUI::SetSepLineColor(DWORD dwSepLineColor)
	{
		if (m_dwSepLineColor == dwSepLineColor)
			return;
		m_dwSepLineColor = dwSepLineColor;
	}

	bool CSplitLayoutUI::IsSepImmMode() const
	{
		return m_bImmMode;
	}

	void CSplitLayoutUI::DoEvent(TEventUI& event)
	{
 		if (event.Type == UIEVENT_BUTTONDOWN) {
 			if( ::PtInRect(&m_rcSeparator, event.ptMouse) ) {
 				m_uButtonState |= UISTATE_CAPTURED;
 				ptLastMouse = event.ptMouse;
 				m_rcNewPos = m_rcSeparator;
 				if( !m_bImmMode && m_pManager ) m_pManager->AddPostPaint(this);
 				return;
 			}
 		} else if (event.Type == UIEVENT_BUTTONUP) {
 			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
 				m_uButtonState &= ~UISTATE_CAPTURED;
 				if( !m_bImmMode && m_pManager )
				{
					// 根据分割条位置获取后面控件指针
					CControlUI* pNext = GetItemAt(m_iSepIndex + 1);
					int nFixed = 0;
					EstimateFixedLength(pNext, m_rcNewPos, nFixed);

					if (m_bVertical)
						pNext->SetFixedHeight(nFixed);
					else
						pNext->SetFixedWidth(nFixed);

					m_pManager->RemovePostPaint(this);
				}
 				NeedParentUpdate();
 			return;
 			}
 		} else if (event.Type == UIEVENT_MOUSEMOVE) {
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				RECT rc = m_rcNewPos;
				
				CDuiRect rcInvalidate = m_rcNewPos;
				CDuiRect rcItemPos = m_rcItem;
				//	暂时使用硬编码限制分割条在控件内边距中
				int nLen = m_iSepWidth*2;
				if (nLen < 10)
					nLen = 10;

				rcItemPos.Deflate(nLen, nLen);
				if (PtInRect(&rcItemPos, event.ptMouse) == FALSE)
					return;
				//加入重绘区域避免移动后不能刷行问题
				//rcInvalidate.Join(m_rcNewPos);
				if (m_bVertical) {
					LONG cy = event.ptMouse.y - ptLastMouse.y;
					rc.top += cy;
					rc.bottom += cy;
				} else{
					LONG cx = event.ptMouse.x - ptLastMouse.x;
					rc.left += cx;
					rc.right += cx;
				}

				//并且判断控件是否在最大最小空间范围内
				// 根据分割条位置获取后面控件指针，主要针对分割条右边或者下边控件
				CControlUI* pNext = GetItemAt(m_iSepIndex + 1);
				int nFixed = 0;
				if (EstimateFixedLength(pNext, rc, nFixed) == false)
					return;

				ptLastMouse = event.ptMouse;
				m_rcNewPos = rc;
				m_rcColRowInfo.SetAt(m_iSepIndex,m_rcNewPos);

				if (m_bImmMode)
				{
					if (m_bVertical)
						pNext->SetFixedHeight(nFixed);
					else
						pNext->SetFixedWidth(nFixed);
					NeedParentUpdate();
				}
				else
				{
					// 加入刷新矩形区域中，方便后续重绘
					rcInvalidate.Join(m_rcNewPos);
					if (m_pManager) m_pManager->Invalidate(rcInvalidate);
				}
			}
 		} else if (event.Type == UIEVENT_SETCURSOR) {
			if (IsEnabled() && m_rcColRowInfo.IsEmpty() ==  false){
				for (int it=0;it<m_rcColRowInfo.GetSize();++it)
				{	
					RECT rcSeparator = m_rcColRowInfo.GetAt(it);
					if (::PtInRect(&rcSeparator, event.ptMouse)) {
						LPCTSTR lpszCursorType = IDC_SIZEWE;
						if (m_bVertical != false)
							lpszCursorType = IDC_SIZENS;
						m_iSepIndex = it;
						memcpy(&m_rcSeparator,&rcSeparator,sizeof(m_rcSeparator));
						::SetCursor(::LoadCursor(NULL, lpszCursorType));
					}
				}
			}
		} else
			CContainerUI::DoEvent(event);
	}

	void CSplitLayoutUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
			CRenderEngine::DrawColor(hDC, m_rcNewPos, m_dwSepLineColor);
		}
	}

	void CSplitLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName,_T("sepimm")) == 0) SetSepImmMode(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("vertical")) == 0 ) SetVertical(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("sepwidth")) == 0) SetSepWidth(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("seplinecolor")) == 0) {
			while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSepLineColor(clrColor);
		}
		else
			CContainerUI::SetAttribute(pstrName,pstrValue);
	}

	void CSplitLayoutUI::SetPos(RECT rc)
	{
 		CControlUI::SetPos(rc);
 		rc = m_rcItem;
 
 		// Adjust for inset
 		rc.left += m_rcInset.left;
 		rc.top += m_rcInset.top;
 		rc.right -= m_rcInset.right;
 		rc.bottom -= m_rcInset.bottom;
 
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
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		if (m_bVertical)
			SetPosInVeri(rc);
		else
			SetPosInHori(rc);
	}

	void CSplitLayoutUI::SetPosInHori(RECT rc)
	{
		/*TStdValArray<RECT> rcColRowInfo(sizeof(RECT),m_rcColRowInfo.GetSize());
		for (int n=0;n<m_rcColRowInfo.GetSize();++n)
			rcColRowInfo.Add(m_rcColRowInfo.GetAt(n));*/

		m_rcColRowInfo.Empty();

		// Determine the width of elements that are sizeable
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

		int nAdjustables = 0;
		int cxFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cx == 0 ) {
				nAdjustables++;
			}
			else {
				if (sz.cx > rc.right - rc.left) sz.cx = (rc.right - rc.left) - 30*m_items.GetSize();
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			cxFixed += sz.cx +  pControl->GetPadding().left + pControl->GetPadding().right;
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * m_iChildPadding;
		if (m_iSepWidth != 0)
			cxFixed += (nEstimateNum - 1) * m_iSepWidth;

		int cxExpand = 0;
		int cxNeeded = 0;
		int cyNeeded = 0;
		if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosX = rc.left;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		}
		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cx -= rcPadding.left;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if( sz.cx == 0 ) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				//////////////////////////////////////////////////////////////////////////
				///corrected by gechunping  on 2014_3_27
				///deleted origin 
				// 				if( iAdjustable == nAdjustables ) {
				// 					sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				// 				}
				///corrected by gechunping  on 2014_3_27
				//////////////////////////////////////////////////////////////////////////

				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if (sz.cx >rc.right - rc.left) sz.cx = (rc.right - rc.left) - 30*m_items.GetSize();
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixedRemaining -= sz.cx;
			}

			sz.cy = pControl->GetFixedHeight();
			if( sz.cy == 0 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
			if( sz.cy < 0 ) sz.cy = 0;
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

			int tmp = sz.cy + rcPadding.top + rcPadding.bottom;
			cyNeeded = (tmp > cyNeeded) ? tmp: cyNeeded;

			//////////////////////////////////////////////////////////////////////////
			///corrected by gechunping  on 2014_3_27
			///origin///  RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy};
			RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx  + rcPadding.left, rc.top + rcPadding.top + sz.cy};
			///corrected by gechunping  on 2014_3_27
			//////////////////////////////////////////////////////////////////////////
			pControl->SetPos(rcCtrl);

			RECT rcSeparator = {rcCtrl.right,rcCtrl.top,rcCtrl.right+m_iSepWidth,rcCtrl.bottom};
			m_rcColRowInfo.Add(rcSeparator);

			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right + m_iSepWidth;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right+ m_iSepWidth;
			szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
		}

		m_rcColRowInfo.Remove(m_rcColRowInfo.GetSize()-1);
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;
		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void CSplitLayoutUI::SetPosInVeri(RECT rc)
	{
		/*TStdValArray<RECT> rcColRowInfo(sizeof(RECT),m_rcColRowInfo.GetSize());
		for (int n=0;n<m_rcColRowInfo.GetSize();++n)
			rcColRowInfo.Add(m_rcColRowInfo.GetAt(n));*/

		m_rcColRowInfo.Empty();
		// Determine the width of elements that are sizeable
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
				if( sz.cy < pControl->GetMinWidth() ) sz.cy = pControl->GetMinWidth();
				if( sz.cy > pControl->GetMaxWidth() ) sz.cy = pControl->GetMaxWidth();
			}
			cyFixed += sz.cy +  pControl->GetPadding().top + pControl->GetPadding().bottom;
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;
		if (m_iSepWidth != 0)
			cyFixed += (nEstimateNum - 1) * m_iSepWidth;

		int cyExpand = 0;
		int cxNeeded = 0;
		int cyNeeded = 0;
		if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			iPosY -= m_pHorizontalScrollBar->GetScrollPos();
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
				// 					sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
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
			///origin///  RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy};
			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy +  rcPadding.bottom };
			///corrected by gechunping  on 2014_3_27
			//////////////////////////////////////////////////////////////////////////
			pControl->SetPos(rcCtrl);

			RECT rcSeparator = {rcCtrl.left,rcCtrl.bottom,rcCtrl.right,rcCtrl.bottom+m_iSepWidth};
			m_rcColRowInfo.Add(rcSeparator);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom + m_iSepWidth;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom + m_iSepWidth;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}

		m_rcColRowInfo.Remove(m_rcColRowInfo.GetSize()-1);
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;
		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	bool CSplitLayoutUI::EstimateFixedLength(CControlUI* pControl, const RECT& rcSeparator, int& nFixed)
	{
		nFixed = 0;
		if (pControl == NULL)
			return false;

		bool bSuccess = false;
		RECT rcPos = pControl->GetPos();
		do 
		{
			if (m_bVertical) {
				rcPos.top = rcSeparator.bottom;
				nFixed = rcPos.bottom - rcPos.top;

				if (nFixed < pControl->GetMinHeight()) {
					nFixed = pControl->GetMinHeight();
					break;
				}
				if (nFixed > pControl->GetMaxHeight()) {
					nFixed = pControl->GetMaxHeight();
					break;
				}
			} else {
				rcPos.left = rcSeparator.right;
				nFixed = rcPos.right - rcPos.left;

				if (nFixed < pControl->GetMinWidth()) {
					nFixed = pControl->GetMinWidth();
					break;
				}
				if (nFixed > pControl->GetMaxWidth()) {
					nFixed = pControl->GetMaxWidth();
					break;
				}
			}

			bSuccess = true;
		} while (FALSE);

		return bSuccess;
	}
}