#include "StdAfx.h"
#include "RichEditEx.h"

namespace DuiLib
{

	CRichEditExUI::CRichEditExUI()
		:CRichEditUI()
	{
		m_iCurSel = -1;

		m_ListInfo.nColumns = 0;
		m_ListInfo.nFont = -1;
		m_ListInfo.uTextStyle = DT_VCENTER; // m_uTextStyle(DT_VCENTER | DT_END_ELLIPSIS)
		m_ListInfo.dwTextColor = 0xFF000000;
		m_ListInfo.dwBkColor = 0xFFF0F0F0;
		m_ListInfo.bAlternateBk = false;
		m_ListInfo.dwSelectedTextColor = 0xFF000000;
		m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
		m_ListInfo.dwHotTextColor = 0xFF000000;
		m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
		m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
		m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
		m_ListInfo.dwLineColor = 0xFF939393;
		m_ListInfo.bShowRowLine = false;
		m_ListInfo.bShowColumnLine = false;
		m_ListInfo.bShowHtml = false;
		m_ListInfo.bMultiExpandable = false;
	}

	CRichEditExUI::~CRichEditExUI()
	{

	}

	LPCTSTR CRichEditExUI::GetClass() const
	{
		return _T("RichEditEx");
	}

	UINT CRichEditExUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP|UIFLAG_SETCURSOR|UIFLAG_WANTRETURN|UIFLAG_IME_COMPOSITION;
	}

	LPVOID CRichEditExUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("RichEditEx")) == 0 ) return static_cast<CRichEditExUI*>(this);
		/*	if( _tcscmp(pstrName, _T("IList")) == 0 ) return static_cast<IListUI*>(this);*/
		if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
		return CRichEditUI::GetInterface(pstrName);
	}


	bool CRichEditExUI::Add(CControlUI* pControl)
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(GetCount());
		}
		return CContainerUI::Add(pControl);
	}

	bool CRichEditExUI::AddAt(CControlUI* pControl, int iIndex)
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
		}

		for(int i = iIndex + 1; i < GetCount(); ++i) {
			CControlUI* p = GetItemAt(i);
			pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		if( m_iCurSel >= iIndex ) m_iCurSel += 1;
		return true;
	}

	TListInfoUI* CRichEditExUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	int CRichEditExUI::GetCurSel() const
	{
		return m_iCurSel;
	}

	bool CRichEditExUI::SelectItem(int iIndex, bool bTakeFocus/* = false*/)
	{
		return true;
	}

	void CRichEditExUI::DoEvent(TEventUI& event)
	{
		CRichEditUI::DoEvent(event);
	}

	bool CRichEditExUI::IsMultiSelect()
	{
		return true;
	}
}
