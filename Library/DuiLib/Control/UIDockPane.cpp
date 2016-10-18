#include "stdafx.h"
#include "UIDockPane.h"

namespace DuiLib
{
	CDockItemUI::CDockItemUI()
	{
		m_bMenuUsed = true;
	}

	CDockItemUI::~CDockItemUI()
	{

	}

	LPCTSTR CDockItemUI::GetClass() const
	{
		return _T("DockItemUI");
	}

	LPVOID CDockItemUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_DOCKITEM) == 0 ) return static_cast<CDockItemUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void CDockItemUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		else if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) 
			{
				m_uButtonState &= ~UISTATE_HOT;

				POINT ptMouse;
				GetCursorPos(&ptMouse);
				ScreenToClient(m_pManager->GetPaintWindow(), &ptMouse);
				if (m_pParent->GetClass() == _T("DockPaneUI") && !::PtInRect(&m_rcItem, ptMouse) )
				{
					static_cast<CDockPaneUI*>(m_pParent)->NotifyLeaveItem(this);
				}	
				Invalidate();
			}
			// return;
		}
		else if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) 	m_uButtonState |= UISTATE_PUSHED;			
				else m_uButtonState &= ~UISTATE_PUSHED;
				//Invalidate();
			}
			if (m_pParent->GetClass() == _T("DockPaneUI")&& ::PtInRect(&m_rcItem, event.ptMouse) )
			{
				double dXRate = double(event.ptMouse.x - m_rcItem.left)/double(m_rcItem.right - m_rcItem.left);
				double dYRate = double(event.ptMouse.y - m_rcItem.top)/double(m_rcItem.bottom - m_rcItem.top);
				static_cast<CDockPaneUI*>(m_pParent)->NotifyEnterItem(this, dXRate, dYRate);
			}	
			return;
		}
		else if( event.Type == UIEVENT_CONTEXTMENU )
		{
			m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
		}
		else if( event.Type == UIEVENT_SETCURSOR ) {
			return;
		}
		else
			CButtonUI::DoEvent(event);
	}

	void CDockItemUI::SetPos(RECT rc)
	{
		CButtonUI::SetPos(rc);
	}



	/////////////////////////////////////////////////////////////
	////
	////

	CDockPaneUI::CDockPaneUI(void)
	{
		m_szItemSize.cx = 60;
		m_szItemSize.cy = 60;
		m_dbMaxRate = 2;
		m_nChangeRange = 3;
		m_DockPane = DOCKPANE_POSITION_BOTTOM;
	}


	CDockPaneUI::~CDockPaneUI(void)
	{
	}

	LPCTSTR CDockPaneUI::GetClass() const
	{
		return _T("DockPaneUI");
	}

	LPVOID CDockPaneUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_DOCKPANE) == 0 ) return static_cast<CDockPaneUI*>(this);
		return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	bool CDockPaneUI::Add(CControlUI* pControl)
	{
		if (pControl->GetInterface(DUI_CTR_DOCKITEM) != NULL)
			return CHorizontalLayoutUI::Add(pControl);

		return false;
	}

	void CDockPaneUI::SetPos(RECT rc)
	{
		if (m_DockPane == DOCKPANE_POSITION_BOTTOM || 
			m_DockPane == DOCKPANE_POSITION_TOP )
		{
			CControlUI::SetPos(rc);
			rc = m_rcItem;
			if( m_items.GetSize() == 0) 
				return;

			//调整inset属性让子控件居中显示
			m_rcInset.left = (rc.right - rc.left - m_szItemSize.cx * m_items.GetSize())/2;

			// 调整rc，减去inset
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;

			// 得到可用的空间
			SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

			int nAdjustables = 0;
			int cxFixed = 0;
			int nEstimateNum = 0;
			for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) 
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
				if( !pControl->IsVisible() ) continue;
				if( pControl->IsFloat() ) continue;
				SIZE sz = pControl->EstimateSize(szAvailable);
				if( sz.cx == 0 ) 
				{
					nAdjustables++;
				}
				else {
					if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
					if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				}
				cxFixed += sz.cx +  pControl->GetPadding().left + pControl->GetPadding().right;
				nEstimateNum++;
			}
			cxFixed += (nEstimateNum - 1) * m_iChildPadding;

			int cxExpand = 0;
			int cxNeeded = 0;
			if( nAdjustables > 0 ) cxExpand = max(0, (szAvailable.cx - cxFixed) / nAdjustables);

			SIZE szRemaining = szAvailable;
			int iPosX = rc.left;

			int iAdjustable = 0;
			int cxFixedRemaining = cxFixed;
			for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) 
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
				if( !pControl->IsVisible() ) continue;

				RECT rcPadding = pControl->GetPadding();
				szRemaining.cx -= rcPadding.left;
				SIZE sz = pControl->EstimateSize(szRemaining);
				if( sz.cx == 0 ) 
				{
					iAdjustable++;
					sz.cx = cxExpand;

					if( iAdjustable == nAdjustables ) 
					{
						sz.cx = max(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
					}
					if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
					if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				}
				else 
				{
					if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
					if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

					cxFixedRemaining -= sz.cx;
				}

				sz.cy = pControl->GetFixedHeight();
				if( sz.cy == 0 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
				if( sz.cy < 0 ) sz.cy = 0;
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

				//将子控件设置到底部或者顶部
				if (m_DockPane == DOCKPANE_POSITION_BOTTOM)
				{
					RECT rcCtrl = { iPosX + rcPadding.left, rc.bottom - sz.cy, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.bottom};
					pControl->SetPos(rcCtrl);
				}
				else
				{
					RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy};
					pControl->SetPos(rcCtrl);
				}
				iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
				cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
				szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
			}
			cxNeeded += (nEstimateNum - 1) * m_iChildPadding;

		} 
		else
		{
			CControlUI::SetPos(rc);
			rc = m_rcItem;

			if( m_items.GetSize() == 0) 
				return;

			//调整inset属性让子控件居中显示
			m_rcInset.top = (rc.bottom - rc.top - m_szItemSize.cy * m_items.GetSize())/2;

			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;

			SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

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


			int cyNeeded = 0;
			int cyExpand = 0;
			if( nAdjustables > 0 ) cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);

			SIZE szRemaining = szAvailable;
			int iPosY = rc.top;
			int iPosX = rc.left;

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

					if( iAdjustable == nAdjustables ) {
						sz.cy = max(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
					} 
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

				//将子控件设置到左侧或者右侧
				if (m_DockPane == DOCKPANE_POSITION_RIGHT)
				{
					RECT rcCtrl = { rc.right - sz.cx, iPosY + rcPadding.top, rc.right, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
					pControl->SetPos(rcCtrl);
				}
				else
				{
					RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
					pControl->SetPos(rcCtrl);
				}


				iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
				cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
				szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
			}
			cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

		}

	}

	void CDockPaneUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("itemsize")) == 0 ) {
			SIZE sz;
			LPTSTR pstr = NULL;
			sz.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			sz.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			SetItemSize(sz);
		}
		else if (_tcscmp(pstrName, _T("maxrate")) == 0)
		{
			SetItemMaxRate(_wtof(pstrValue));
		}
		else if (_tcscmp(pstrName, _T("range")) == 0)
		{
			SetChangeRange(_ttoi(pstrValue));
		}
		else if (_tcscmp(pstrName, _T("positon")) == 0)
		{
			SetPositon(DOCKPANE_POSITION(_ttoi(pstrValue)));
		}
		else 
			CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CDockPaneUI::DoEvent(TEventUI& event)
	{
		CHorizontalLayoutUI::DoEvent(event);
	}

	void CDockPaneUI::SetItemSize(SIZE sz)
	{
		m_szItemSize = sz;
		for (int i = 0; i < m_items.GetSize(); i++)
		{
			static_cast<CControlUI*>(m_items[i])->SetFixedWidth(m_szItemSize.cx);
			static_cast<CControlUI*>(m_items[i])->SetFixedHeight(m_szItemSize.cy);
		}
	}

	SIZE CDockPaneUI::GetItemSize() const
	{
		return m_szItemSize;
	}


	void CDockPaneUI::SetItemMaxRate(double dRate)
	{
		if (dRate >3 || dRate < 1)
			return;
		m_dbMaxRate = dRate;
	}

	double CDockPaneUI::GetItemMaxRate() const
	{
		return m_dbMaxRate;
	}

	void CDockPaneUI::SetChangeRange(int nNum)
	{
		if (nNum > 5 || nNum <2)
			return;
		m_nChangeRange = nNum;
	}

	int	CDockPaneUI::GetChangeRange() const
	{
		return m_nChangeRange;
	}

	void CDockPaneUI::SetPositon(DOCKPANE_POSITION DockPanePosition)
	{
		if (DockPanePosition < DOCKPANE_POSITION_LEFT || 
			DockPanePosition > DOCKPANE_POSITION_BOTTOM )
			m_DockPane = DOCKPANE_POSITION_BOTTOM;
		else
			m_DockPane = DockPanePosition;
	}

	DOCKPANE_POSITION CDockPaneUI::GetPositon() const
	{
		return m_DockPane;
	}

	void CDockPaneUI::NotifyEnterItem(CDockItemUI* pItem, double dXRate, double dYRate)
	{
		int index = GetItemIndex(pItem);
		CControlUI* pFirstItem = GetItemAt(0);
		if (index == -1 || pFirstItem == NULL)
			return;

		if (m_DockPane == DOCKPANE_POSITION_BOTTOM || 
			m_DockPane  == DOCKPANE_POSITION_TOP )
		{
			int nMouseX = int(pFirstItem->GetPos().right + index * m_szItemSize.cx + m_szItemSize.cx * dXRate);

			for( int it = 0; it < m_items.GetSize(); it++ ) 
			{
				CControlUI* pItem = static_cast<CControlUI*>(m_items[it]);
				RECT rcItem = pItem->GetPos();
				int nItemCenterX = rcItem.left + (rcItem.right - rcItem.left)/2;

				int nDistance = abs(nItemCenterX - nMouseX);
				double dbRate = (1- (double)(nDistance) / (double)(m_szItemSize.cx * m_nChangeRange)) *
					(m_dbMaxRate - 1) + 1;

				if (dbRate < 1)
					dbRate = 1;
				if (dbRate > m_dbMaxRate)
					dbRate = m_dbMaxRate;

				pItem->SetFixedHeight(int(m_szItemSize.cy * dbRate));
				pItem->SetFixedWidth(int(m_szItemSize.cx * dbRate));	
			}
		} 
		else
		{
			int nMouseY = int(pFirstItem->GetPos().bottom + index * m_szItemSize.cy + m_szItemSize.cy * dYRate);

			for( int it = 0; it < m_items.GetSize(); it++ ) 
			{
				CControlUI* pItem = static_cast<CControlUI*>(m_items[it]);
				RECT rcItem = pItem->GetPos();
				int nItemCenterY = rcItem.top + (rcItem.bottom - rcItem.top)/2;

				int nDistance = abs(nItemCenterY - nMouseY);
				double dbRate = (1- (double)(nDistance) / (double)(m_szItemSize.cy * m_nChangeRange)) *
					(m_dbMaxRate - 1) + 1;

				if (dbRate < 1)
					dbRate = 1;
				if (dbRate > m_dbMaxRate)
					dbRate = m_dbMaxRate;

				pItem->SetFixedHeight(int(m_szItemSize.cy * dbRate));
				pItem->SetFixedWidth(int(m_szItemSize.cx * dbRate));	
			}
		}

	}

	void CDockPaneUI::NotifyLeaveItem(CDockItemUI* pItem)
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) 
		{
			CControlUI* pItem = static_cast<CControlUI*>(m_items[it]);
			pItem->SetFixedHeight(m_szItemSize.cy);
			pItem->SetFixedWidth(m_szItemSize.cx);
		}
	}
}

