#include "stdafx.h"

namespace DuiLib 
{
	CPictureUI::CPictureUI(void)
	{
		m_hIcon = NULL;
	}

	CPictureUI::~CPictureUI(void)
	{
		if (m_hIcon != NULL)
		{
			DestroyIcon(m_hIcon);
			m_hIcon = NULL;
		}
	}

	LPCTSTR CPictureUI::GetClass() const
	{
		return _T("PictureUI");
	}

	LPVOID CPictureUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_PICTURE) == 0 ) return static_cast<CPictureUI*>(this);
		else
			return  CControlUI::GetInterface(pstrName);
	}

	void CPictureUI::DoEvent( TEventUI& event )
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) 
		{
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS ) {
			m_bFocused = true;
			return;
		} else if ( event.Type == UIEVENT_KILLFOCUS ) {
			m_bFocused = false;
			return;
		} else if (event.Type == UIEVENT_BUTTONDOWN) {
			if (IsEnabled()) {
				if (m_pManager)
					m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
			}
			return;
		}else if (event.Type == UIEVENT_DBLCLICK) {
			if (IsEnabled()) {
				if (m_pManager)
					m_pManager->SendNotify(this, DUI_MSGTYPE_DBCLICK);
			}
			return;
		} else
			CControlUI::DoEvent(event);
	}

	void CPictureUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("foreimage")) == 0)
			SetForeImage(pstrValue);
		else 
			CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CPictureUI::SetIcon(HICON hIcon)
	{
		if (hIcon == NULL)
			return;
	
 		ICONINFO IconInfo;
 		GetIconInfo(hIcon, &IconInfo);

		m_hIcon = CreateIconIndirect(&IconInfo);
		Invalidate();
	}

	void CPictureUI::SetForeImage( LPCTSTR pStrImage )
	{
		if (m_sForeImage == pStrImage)
			return;
		m_sForeImage = pStrImage;
		Invalidate();
	}

	void CPictureUI::PaintStatusImage(HDC hDC)
	{
		if (m_hIcon != NULL) {
			DrawIconEx(hDC, m_rcItem.left, m_rcItem.top, m_hIcon, GetWidth(), GetHeight(), 0, NULL, DI_NORMAL | DI_COMPAT);
		}
		if (!m_sForeImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)m_sForeImage) ) m_sForeImage.Empty();
		}
	}
}