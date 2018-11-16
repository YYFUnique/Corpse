#include "StdAfx.h"
#include "UIFileList.h"

namespace DuiLib
{
	CFileListItemUI::CFileListItemUI()
		: CListContainerElementUI()
	{
	
	}

	CFileListItemUI::~CFileListItemUI()
	{

	}

	LPCTSTR CFileListItemUI::GetClass() const
	{
		return _T("FileListItemUI");
	}

	LPVOID CFileListItemUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_FILELISTITEM) == 0) return static_cast<CFileListItemUI*>(this);
		else
			return CListContainerElementUI::GetInterface(pstrName);
	}

	void CFileListItemUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_BUTTONDOWN) {
			if (IsEnabled()) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
				if (m_pOwner != NULL) 
					m_pOwner->SelectItem(m_iIndex);
				Invalidate();
			}
			return;
		} else if (event.Type == UIEVENT_MOUSEENTER) {
			if (IsEnabled()) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		} else if (event.Type == UIEVENT_MOUSELEAVE) {
			if ((m_uButtonState & UISTATE_HOT) != 0){
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		} else
			CListContainerElementUI::DoEvent(event);
	}

	//按照Hor控件编写  每个子项对应head的宽度
	void CFileListItemUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		if( m_items.IsEmpty() ) return;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		TListInfoUI* pListInfo = GetOwner()->GetListInfo();
		CDuiRect rcHeader = pListInfo->rcColumn[0];
		if (rcHeader.IsRectEmpty())
			return;

		int iPosX = rc.left;

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
			}
			else {
				RECT rcCtrl = {iPosX,
										rc.top,
										iPosX + pListInfo->rcColumn[it].right - pListInfo->rcColumn[it].left ,
										rc.bottom};
				iPosX = rcCtrl.right;
				pControl->SetPos(rcCtrl);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////
	////
	/////////////////////////////////////////////////////////////////////

	CFileListUI::CFileListUI()
		: m_pSelectRect(NULL)
	{
		
	}

	CFileListUI::~CFileListUI()
	{

	}

	LPCTSTR CFileListUI::GetClass() const
	{
		return _T("FileListUI");
	}

	LPVOID CFileListUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_FILELIST) == 0)
			return static_cast<CFileListUI*>(this);
		else
			return CListUI::GetInterface(pstrName);
	}

	void CFileListUI::DoEvent(TEventUI &event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) 
		{
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CListUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_BUTTONDOWN) {
			if (IsEnabled()) {
				m_ptDown = event.ptMouse;
				m_uButtonState |= UISTATE_PUSHED;
			}
			return;
		} else if (event.Type == UIEVENT_BUTTONUP) {
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_SELECTED) == 0) {
					SelectItem(INVALID_ITEM_INDEX);
					//m_pManager->SendNotify(this,)
				}
				m_uButtonState &= ~UISTATE_SELECTED;
				m_uButtonState &= ~UISTATE_PUSHED;
				m_ptMove.x = m_ptMove.y = 0;

				if (m_pSelectRect != NULL) {
					m_pSelectRect->SetVisible(false);
					Remove(m_pSelectRect);
					m_pSelectRect = NULL;
				}
			}
			return;
		} else if (event.Type == UIEVENT_MOUSEMOVE) {
			if (IsEnabled()) {
				if (m_uButtonState & UISTATE_PUSHED) {
					if (!(m_uButtonState & UISTATE_SELECTED))
						SelectItem(INVALID_ITEM_INDEX);

					m_uButtonState |= UISTATE_SELECTED;
					m_ptMove = event.ptMouse;

					//	如果支持多选，那么支持拖动鼠标框选的方式
					if (m_bMultiSel) {
						CDuiRect rcSelect(m_ptDown.x,m_ptDown.y,m_ptMove.x,m_ptMove.y);
						rcSelect.Normalize();

						if (m_pSelectRect == NULL)
						{
							m_pSelectRect = new CControlUI;
							Add(m_pSelectRect);

							m_pSelectRect->SetFloat(true);
							m_pSelectRect->SetBkColor(0x25000000);
						}
						m_pSelectRect->SetPos(rcSelect);
					}
				}
			}
			return;
		} else
			CListUI::DoEvent(event);
	}
}