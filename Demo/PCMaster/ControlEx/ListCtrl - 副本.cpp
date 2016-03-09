#include "../stdafx.h"
#include "ListCtrl.h"
#include "../PCMaster.h"
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

	CListCtrlItemUI::CListCtrlItemUI()
	{

	}

	LPCTSTR CListCtrlItemUI::GetClass() const
	{
		return _T("CListCtrlItemUI");
	}

	LPVOID CListCtrlItemUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListCtrlItem")) == 0 )  return static_cast<CListCtrlItemUI*>(this);
		return CListContainerElementUI::GetInterface(pstrName);
	}

	UINT CListCtrlItemUI::GetControlFlags() const
	{
		return UIFLAG_WANTRETURN;
	}

	void CListCtrlItemUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(_T("IListOwner")));
	}

	void CListCtrlItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
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

	const CDuiString& CListCtrlItemUI::GetItemPath()
	{
		return m_strItemPath;
	}

	void CListCtrlItemUI::SetItemPath(LPCTSTR lpszItemPath)
	{
		m_strItemPath = lpszItemPath;
	}

	const CDuiString& CListCtrlItemUI::GetSelectedImage()
	{
		return m_strSelectedImage;
	}

	void CListCtrlItemUI::SetSelectedImage(LPCTSTR lpszSelectedImage)
	{
		m_strSelectedImage = lpszSelectedImage;
	}

	DWORD CListCtrlItemUI::GetSelectedColor()
	{
		return m_dwSelectedColor;
	}

	void CListCtrlItemUI::SetSelectedColor(DWORD dwSelectedColor)
	{
		m_dwSelectedColor = dwSelectedColor;
	}

	const CDuiString& CListCtrlItemUI::GetHotImage()
	{
		return m_strHotImage;
	}

	void CListCtrlItemUI::SetHotImage(LPCTSTR lpszHotImage)
	{
		m_strHotImage = lpszHotImage;
	}

	const CDuiString& CListCtrlItemUI::GetPushImage()
	{
		return m_strPushImage;
	}

	void CListCtrlItemUI::SetPushImage(LPCTSTR lpszPushImage)
	{
		m_strPushImage = lpszPushImage;
	}

	void CListCtrlItemUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		PaintStatusImage(hDC);
		CContainerUI::DoPaint(hDC, rcPaint);
	}

	void CListCtrlItemUI::PaintStatusImage(HDC hDC)
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

	void CListCtrlItemUI::DoEvent(TEventUI& event)
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

	///////////////////////////////////CListCtrlUI///////////////////////////////////////////////
	CListCtrlUI::CListCtrlUI(CPaintManagerUI& PaintManager)
		:CTileListCtrlUI(PaintManager)
	{
		SetItemSize(CSize(80,85));

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
		 if(_tcscmp(pstrName,_T("selectedimage")) == 0) SetSelectedImage(pstrValue);
		else if(_tcscmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
		/*else if(_tcscmp(pstrName, _T("pushedimage")) == 0) SetPushedImage(pstrValue);*/
		else
			return __super::SetAttribute(pstrName,pstrValue);
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

	bool CListCtrlUI::AddNode(LPARAM lParam)
	{
		const ListItemInfo* pListItemInfo = (PListItemInfo)lParam;
		CListCtrlItemUI* pItemList = NULL;
		CPCMaster* pMainDlg = (CPCMaster*)GetMainApp();
		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pItemList = (CListCtrlItemUI*)m_DialogBuilder.Create(_T("ListCtrl.xml"),0,pMainDlg,&m_PaintManager,NULL);
		else
			pItemList = (CListCtrlItemUI*)m_DialogBuilder.Create(pMainDlg,&m_PaintManager);

		ASSERT(pItemList);

		CLabelUI* pLabelName = (CLabelUI*)m_PaintManager.FindSubControlByName(pItemList,_T("Icon"));
		pLabelName->SetTag((UINT_PTR)pItemList);
		pLabelName->OnEvent += MakeDelegate(&OnBtnEvent);
		LPCTSTR lpszItemIcon = pListItemInfo->strListItemIcon;
		SHFILEINFO shFileInfo;
		if (pListItemInfo->strListItemIcon.IsEmpty())
		{
			SHGetFileInfo(pListItemInfo->strListItemPath,FILE_ATTRIBUTE_NORMAL,
							&shFileInfo,sizeof(shFileInfo),SHGFI_DISPLAYNAME);
		}
		pLabelName->SetBkImage(lpszItemIcon);

		pLabelName = (CLabelUI*)m_PaintManager.FindSubControlByName(pItemList,_T("LabelName"));
		pLabelName->SetTag((UINT_PTR)pItemList);
		pLabelName->OnEvent += MakeDelegate(&OnBtnEvent);
		LPCTSTR lpszItemName = pListItemInfo->strListItemName;
		if (pListItemInfo->strListItemName.IsEmpty())
			lpszItemName = shFileInfo.szDisplayName;
		pLabelName->SetText(lpszItemName);

		pItemList->SetItemPath(pListItemInfo->strListItemPath);
		
		return Add(pItemList);
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