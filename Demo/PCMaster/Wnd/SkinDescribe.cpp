#include "../stdafx.h"
#include "SkinDescribe.h"

CSkinDescribeUI::CSkinDescribeUI()
:m_uButtonState(0)
, m_dwDelayDeltaY(0)
, m_dwDelayNum(0)
, m_dwDelayLeft(0)
{
	SetItemSize(CSize(182, 152));
	CDialogBuilder builder;
	CContainerUI* pSkinDescribe = static_cast<CContainerUI*>(builder.Create(_T("SkinDescribe.xml"), (UINT)0));
	if( pSkinDescribe != NULL )
	{
		for(int i = 0; i < 10; ++i) 
		{
			if( pSkinDescribe == NULL ) pSkinDescribe = static_cast<CContainerUI*>(builder.Create());
			if( pSkinDescribe != NULL ) {
				this->Add(pSkinDescribe);
				pSkinDescribe = NULL;
			}
			else {
				this->RemoveAll();
				return;
			}
		}
	}
}

CSkinDescribeUI::~CSkinDescribeUI()
{

}

void CSkinDescribeUI::DoEvent(TEventUI &event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else CTileLayoutUI::DoEvent(event);
		return;
	}
	if( event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			POINT pt = m_pManager->GetMousePos();
			LONG cy = (pt.y - m_ptLastMouse.y);
			m_ptLastMouse = pt;
			SIZE sz = GetScrollPos();
			sz.cy -= cy;
			SetScrollPos(sz);
			return;
		}
		else if( m_dwDelayLeft > 0 ) {
			--m_dwDelayLeft;
			SIZE sz = GetScrollPos();
			LONG lDeltaY =  (LONG)(CalculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
			if( (lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != GetScrollRange().cy ) ) {
				sz.cy -= lDeltaY;
				SetScrollPos(sz);
				return;
			}
		}
		m_dwDelayDeltaY = 0;
		m_dwDelayNum = 0;
		m_dwDelayLeft = 0;
		m_pManager->KillTimer(this, SCROLL_TIMERID);
		return;
	}
	if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
	{
		m_uButtonState |= UISTATE_CAPTURED;
		m_ptLastMouse = event.ptMouse;
		m_dwDelayDeltaY = 0;
		m_dwDelayNum = 0;
		m_dwDelayLeft = 0;
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			m_uButtonState &= ~UISTATE_CAPTURED;
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			if( m_ptLastMouse.y != event.ptMouse.y ) {
				m_dwDelayDeltaY = (event.ptMouse.y - m_ptLastMouse.y);
				if( m_dwDelayDeltaY > 120 ) m_dwDelayDeltaY = 120;
				else if( m_dwDelayDeltaY < -120 ) m_dwDelayDeltaY = -120;
				m_dwDelayNum = (DWORD)sqrt((double)abs(m_dwDelayDeltaY)) * 5;
				m_dwDelayLeft = m_dwDelayNum;
			}
			else 
				m_pManager->KillTimer(this, SCROLL_TIMERID);
		}
		return;
	}
	if( event.Type == UIEVENT_SCROLLWHEEL )
	{
		LONG lDeltaY = 0;
		if( m_dwDelayNum > 0 ) lDeltaY =  (LONG)(CalculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
		switch( LOWORD(event.wParam) ) {
				case SB_LINEUP:
					if( m_dwDelayDeltaY >= 0 ) m_dwDelayDeltaY = lDeltaY + 8;
					else m_dwDelayDeltaY = lDeltaY + 12;
					break;
				case SB_LINEDOWN:
					if( m_dwDelayDeltaY <= 0 ) m_dwDelayDeltaY = lDeltaY - 8;
					else m_dwDelayDeltaY = lDeltaY - 12;
					break;
		}
		if( m_dwDelayDeltaY > 100 ) m_dwDelayDeltaY = 100;
		else if( m_dwDelayDeltaY < -100 ) m_dwDelayDeltaY = -100;
		m_dwDelayNum = (DWORD)sqrt((double)abs(m_dwDelayDeltaY)) * 5;
		m_dwDelayLeft = m_dwDelayNum;
		m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
		return;
	}
	CTileLayoutUI::DoEvent(event);
}

LPCTSTR CSkinDescribeUI::GetClass() const
{
	return _T("CSkinDescribeUI");
}

LPVOID CSkinDescribeUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("SkinDescribe")) == 0 ) return static_cast<CSkinDescribeUI*>(this);
	return CTileLayoutUI::GetInterface(pstrName);
}