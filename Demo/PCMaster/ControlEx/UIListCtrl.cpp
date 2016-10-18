#include "../stdafx.h"
#include "UIListCtrl.h"
#include "../PCMaster.h"
#include <shellapi.h>

namespace DuiLib
{
	IListCtrlItemUI::IListCtrlItemUI()
		:m_pOwner(NULL)
		,m_iIndex(-1)
		,m_bSelected(false)
	{

	}

	void IListCtrlItemUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(_T("IListOwner")));
	}

	IListOwnerUI* IListCtrlItemUI::GetOwner()
	{
		return m_pOwner;
	}

	int IListCtrlItemUI::GetIndex() const
	{
		return m_iIndex;
	}

	void IListCtrlItemUI::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	bool IListCtrlItemUI::IsSelected() const
	{
		return m_bSelected;
	}

	bool IListCtrlItemUI::IsExpanded() const
	{
		return false;
	}

	bool IListCtrlItemUI::Expand(bool bExpand /*= true*/)
	{
		return false;
	}

	void IListCtrlItemUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		
	}

	///////////////////////////////////CListCtrlUI///////////////////////////////////////////////
	CListCtrlUI::CListCtrlUI(CPaintManagerUI& PaintManager)
		:CTileListCtrlUI(PaintManager)
	{
		SetItemSize(CSize(70,75));

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

	LPCTSTR CListCtrlUI::GetClass() const
	{
		return _T("CListCtrlUI");
	}

	LPVOID CListCtrlUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
		if (_tcscmp(pstrName,_T("ListCtrl")) == 0)		return static_cast<CListCtrlUI*>(this);
		else 
			return CTileLayoutUI::GetInterface(pstrName);
	}

	void CListCtrlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName,_T("itemfont")) == 0) SetItemFont(_ttoi(pstrValue));
		else if(_tcscmp(pstrName,_T("itemselectedimage")) == 0) SetSelectedImage(pstrValue);
		else if(_tcscmp(pstrName, _T("itemhotimage")) == 0) SetHotImage(pstrValue);
		else if(_tcscmp(pstrName, _T("itempushedimage")) == 0) SetPushedImage(pstrValue);
		else
			return __super::SetAttribute(pstrName,pstrValue);
	}

	void CListCtrlUI::SetItemFont(int iFont)
	{
		if (m_ListInfo.nFont == iFont)
			return;
		m_ListInfo.nFont = iFont;
		Invalidate();
	}

	void CListCtrlUI::SetSelectedImage(LPCTSTR lpszSelectedImage)
	{
		if (m_ListInfo.sSelectedImage == lpszSelectedImage)
			return;
		m_ListInfo.sSelectedImage = lpszSelectedImage;
		Invalidate();
	}

	void CListCtrlUI::SetHotImage(LPCTSTR lpszHotImage)
	{
		if (m_ListInfo.sHotImage == lpszHotImage)
			return;
		m_ListInfo.sHotImage = lpszHotImage;
		Invalidate();
	}

	void CListCtrlUI::SetPushedImage(LPCTSTR lpszPushedImage)
	{
		if (m_ListInfo.sPushedImage == lpszPushedImage)
			return;
		m_ListInfo.sPushedImage = lpszPushedImage;
		Invalidate();
	}

	void CListCtrlUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CTileLayoutUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_KEYDOWN)
		{
			switch(event.chKey){
				case VK_UP:
				case VK_DOWN:
				case VK_LEFT:
				case VK_RIGHT:
						SelectItemByKey(event.chKey);
					break;
			}
			return;
		}

		CTileLayoutUI::DoEvent(event);
	}

	TListInfoUI* CListCtrlUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	void CListCtrlUI::SelectItemByKey(TCHAR szKey)
	{
		int nCol = GetColumns();
		int iOldSel = m_iCurSel;
		if (VK_UP == szKey)
			iOldSel -= iOldSel/nCol == 0 ? 0 : nCol;
		else if (VK_DOWN == szKey && iOldSel + nCol < GetCount())
			iOldSel += iOldSel/nCol == GetCount()/nCol ? 0 : nCol;
		else if (VK_LEFT == szKey)
			iOldSel -= iOldSel%nCol == 0 ? 0 : 1;
		else if (VK_RIGHT ==szKey && iOldSel<GetCount()-1)
			iOldSel += iOldSel%nCol == nCol-1 ? 0 : 1;
		SelectItem(iOldSel,true);
	}
} //namespace DuiLib