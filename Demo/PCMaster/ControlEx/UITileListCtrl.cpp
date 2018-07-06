#include "../stdafx.h"
#include "UITileListCtrl.h"

namespace DuiLib
{
	CTileListCtrlUI::CTileListCtrlUI(CPaintManagerUI& PaintManager)
		:m_PaintManager(PaintManager)
	{
		SetItemSize(CDuiSize(80,85));
		m_ListInfo.nColumns = 0;
		m_ListInfo.nFont = -1;
		m_ListInfo.uTextStyle = DT_VCENTER; // m_uTextStyle(DT_VCENTER | DT_END_ELLIPSIS)
		m_ListInfo.dwTextColor = 0xFF000000;
		m_ListInfo.dwBkColor = 0;
		m_ListInfo.bAlternateBk = false;
		m_ListInfo.dwSelectedTextColor = 0xFF000000;
		m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
		m_ListInfo.dwHotTextColor = 0xFF000000;
		m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
		m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
		m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
		m_ListInfo.dwLineColor = 0;
		m_ListInfo.bShowHtml = false;
		m_ListInfo.bMultiExpandable = false;
		m_ListInfo.bShowColumnLine=false;
		::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
		::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
	}

	CTileListCtrlUI::~CTileListCtrlUI()
	{

	}

	LPCTSTR CTileListCtrlUI::GetClass() const
	{
		return _T("CTileListCtrl");
	}

	LPVOID CTileListCtrlUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName,_T("IListOwner")) == 0) return static_cast<IListOwnerUI*>(this);
		if (_tcsicmp(pstrName,_T("TileListCtrlUI")) == 0 ) return static_cast<CTileListCtrlUI*>(this);
		else
			return CTileLayoutUI::GetInterface(pstrName);
	}

	TListInfoUI* CTileListCtrlUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	int CTileListCtrlUI::GetCurSel() const
	{
		return m_iCurSel;
	}

	void CTileListCtrlUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CTileLayoutUI::DoEvent(event);
			return;
		}

		CTileLayoutUI::DoEvent(event);
	}

	bool CTileListCtrlUI::Add(CControlUI* pControl)
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));

		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(GetCount());
		}

		return CTileLayoutUI::Add(pControl);
	}

	bool CTileListCtrlUI::AddNode(LPARAM lParam)
	{
		return true;
	}

	bool CTileListCtrlUI::IsMultiSelect()
	 {
		 return false;
	 }

	bool CTileListCtrlUI::SelectItem(int iIndex, bool bTakeFocus/* = false*/)
	{
		if( iIndex == m_iCurSel ) return true;

		int iOldSel = m_iCurSel;
		if( m_iCurSel >= 0 ) {
			CControlUI* pControl = GetItemAt(m_iCurSel);
			if( pControl != NULL) {
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem != NULL ) pListItem->Select(false);
			}

			m_iCurSel = -1;
		}
		if( iIndex < 0 ) return false;

		CControlUI* pControl = GetItemAt(iIndex);
		if( pControl == NULL ) return false;
		if( !pControl->IsVisible() ) return false;
		if( !pControl->IsEnabled() ) return false;

		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;
		m_iCurSel = iIndex;
		if( !pListItem->Select(true) ) {
			m_iCurSel = -1;
			return false;
		}
		EnsureVisible(m_iCurSel);
		if( bTakeFocus ) pControl->SetFocus();
		if( m_pManager != NULL ) {
			m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);
		}

		return true;
	}

	void CTileListCtrlUI::EnsureVisible(int iIndex)
	{
		if( m_iCurSel < 0 ) return;
		RECT rcItem = GetItemAt(iIndex)->GetPos();
		RECT rcList = GetPos();
		RECT rcListInset = GetInset();

		rcList.left += rcListInset.left;
		rcList.top += rcListInset.top;
		rcList.right -= rcListInset.right;
		rcList.bottom -= rcListInset.bottom;

		CScrollBarUI* pHorizontalScrollBar = GetHorizontalScrollBar();
		if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

		int iPos = GetScrollPos().cy;
		if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
		int dx = 0;
		if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
		if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
		Scroll(0, dx);
	}

	void CTileListCtrlUI::Scroll(int dx, int dy)
	{
		if( dx == 0 && dy == 0 ) return;
		SIZE sz = GetScrollPos();
		SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
	}
}
