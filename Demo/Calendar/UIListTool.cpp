#include "stdafx.h"
#include "UIListTool.h"
#include <shellapi.h>

namespace DuiLib
{
	static bool OnBtnEvent(void* event){
		CControlUI* pControl = ((TEventUI*)event)->pSender;
		if( pControl != NULL ) {
			CControlUI* pElement = (CListContainerElementUI*)(pControl->GetTag());
			if( pElement != NULL ) pElement->DoEvent(*(TEventUI*)event);
		}
		return false;
	}

	CListToolItemUI::CListToolItemUI()
	{

	}

	LPCTSTR CListToolItemUI::GetClass() const
	{
		return _T("CListToolItemUI");
	}

	LPVOID CListToolItemUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LISTTOOLITEM) == 0 )  return static_cast<CListToolItemUI*>(this);
		return CListContainerElementUI::GetInterface(pstrName);
	}

	UINT CListToolItemUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	void CListToolItemUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(_T("IListOwner")));
	}

	void CListToolItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("itempath")) == 0) SetItemPath(pstrValue);
		else if(_tcscmp(pstrName,_T("selectedimage")) == 0) SetSelectedImage(pstrValue);
		else if(_tcscmp(pstrName, _T("selectedcolor")) == 0)  {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedColor(clrColor);
		}
		else if(_tcscmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
		else if(_tcscmp(pstrName, _T("pushedimage")) == 0) SetPushImage(pstrValue);
		else
			return CListContainerElementUI::SetAttribute(pstrName,pstrValue);
	}

	const CDuiString& CListToolItemUI::GetItemPath()
	{
		return m_strItemPath;
	}

	void CListToolItemUI::SetItemPath(LPCTSTR lpszItemPath)
	{
		m_strItemPath = lpszItemPath;
	}

	const CDuiString& CListToolItemUI::GetSelectedImage()
	{
		return m_strSelectedImage;
	}

	void CListToolItemUI::SetSelectedImage(LPCTSTR lpszSelectedImage)
	{
		m_strSelectedImage = lpszSelectedImage;
	}

	DWORD CListToolItemUI::GetSelectedColor()
	{
		return m_dwSelectedColor;
	}

	void CListToolItemUI::SetSelectedColor(DWORD dwSelectedColor)
	{
		m_dwSelectedColor = dwSelectedColor;
	}

	const CDuiString& CListToolItemUI::GetHotImage()
	{
		return m_strHotImage;
	}

	void CListToolItemUI::SetHotImage(LPCTSTR lpszHotImage)
	{
		m_strHotImage = lpszHotImage;
	}

	const CDuiString& CListToolItemUI::GetPushImage()
	{
		return m_strPushImage;
	}

	void CListToolItemUI::SetPushImage(LPCTSTR lpszPushImage)
	{
		m_strPushImage = lpszPushImage;
	}

	void CListToolItemUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		PaintStatusImage(hDC);
		CContainerUI::DoPaint(hDC, rcPaint);
	}

	void CListToolItemUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if ( IsSelected() !=0 ){
			if (!m_strSelectedImage.IsEmpty()){
				if (!DrawImage(hDC,(LPCTSTR)m_strSelectedImage)){
					m_strSelectedImage.Empty();
				}
			}
			else if(m_dwSelectedColor != 0) 
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedColor));
		}

		if ((m_uButtonState & UISTATE_PUSHED) != 0){
			if (!m_strPushImage.IsEmpty()){
				if (!DrawImage(hDC,(LPCTSTR)m_strPushImage)){
					m_strPushImage.Empty();
				}
			}
			return;
		}

		if ((m_uButtonState & UISTATE_HOT) !=0){
			if (!m_strHotImage.IsEmpty()){
				if( !DrawImage(hDC, (LPCTSTR)m_strHotImage)){
					m_strHotImage.Empty();
				}
			}
			return;
		}
	}

	void CListToolItemUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CListContainerElementUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
		{
			if( IsEnabled() ){
				Select();
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_KEYDOWN && event.chKey == VK_RETURN)
		{
			if (IsEnabled())
				m_pManager->SendNotify(this,DUI_MSGTYPE_ITEMSELECT);
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if (event.Type == UIEVENT_DBLCLICK)
		{
			if (IsEnabled()){
				Select();
				m_pManager->SendNotify(this,DUI_MSGTYPE_ITEMSELECT);
				Invalidate();
			}
			return ;
		}

		CListContainerElementUI::DoEvent(event);
	}

	///////////////////////////////////CListToolUI///////////////////////////////////////////////
	CListToolUI::CListToolUI(CPaintManagerUI& PaintManager,SIZE& ItemSize)
		:CTileCtrlUI(PaintManager,ItemSize)
	{
		SetItemSize(ItemSize);
	}

	LPCTSTR CListToolUI::GetClass() const
	{
		return _T("CListToolUI");
	}

	LPVOID CListToolUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
		if (_tcscmp(pstrName,_T("ListTool")) == 0)		return static_cast<CListToolUI*>(this);
		else 
			return CTileLayoutUI::GetInterface(pstrName);
	}

	bool CListToolUI::AddNode(LPARAM lParam,LPCTSTR lpszXmlFile)
	{
		return false;
	}

	void CListToolUI::DoEvent(TEventUI& event)
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

	void CListToolUI::SelectItemByKey(TCHAR szKey)
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