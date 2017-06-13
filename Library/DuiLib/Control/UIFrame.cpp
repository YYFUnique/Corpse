#include "StdAfx.h"

namespace DuiLib
{
	CFrameUI::CFrameUI()
		: m_dwPeroid(1000),
		  m_dwTotal(1),
		  m_dwFrame(0)
	{
		ZeroMemory(&m_ptTopLeft, sizeof(m_ptTopLeft));
	}

	CFrameUI::~CFrameUI()
	{

	}

	LPCTSTR CFrameUI::GetClass() const
	{
		return _T("FrameSeqUI");
	}

	LPVOID CFrameUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_FRAME) == 0)	return static_cast<CFrameUI*>(this);
		else
			return CLabelUI::GetInterface(pstrName);
	}

	UINT CFrameUI::GetControlFlags() const
	{
		return IsEnabled() ? UIFLAG_SETCURSOR : 0;
	}

	void CFrameUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_TIMER )
		{
			if( IsEnabled() ) {
				if ( event.wParam == FRAME_ANIMATE_TIME_ID ){
					//目前只考虑横向设置，且高度一致的图片
					m_ptTopLeft.x = m_dwFrame * (m_rcItem.right - m_rcItem.left);
					m_ptTopLeft.y = 0;
					++m_dwFrame;

					//帧动画绘制完成，关闭定时器
					if (m_dwFrame >= m_dwTotal)
						m_pManager->KillTimer(this, FRAME_ANIMATE_TIME_ID);

					Invalidate();
				}else{
					m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam, true);
				}				
			}
			 return;
		}
		CLabelUI::DoEvent(event);
	}

	void CFrameUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		CLabelUI::DoPaint(hDC, rcPaint);

		CDuiRect rcRect = m_rcItem;

		CDuiString strFrameImg;
		strFrameImg.Format(_T("file='%s' source='%d,%d,%d,%d'"),(LPCTSTR)m_strFrameImg, m_ptTopLeft.x,m_ptTopLeft.y,
								m_ptTopLeft.x + rcRect.GetWidth(), m_ptTopLeft.y + rcRect.GetHeight() );

		DrawImage(hDC, strFrameImg);
	}

	void CFrameUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("time")) == 0) SetPeriod(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("frame")) == 0) SetFrameCount(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("frameimg")) == 0) SetFrameImage(pstrValue);
		else
			CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CFrameUI::SetPeriod(DWORD dwPeroid)
	{
		m_dwPeroid = dwPeroid;
	}

	void CFrameUI::SetFrameCount(DWORD dwTotal)
	{
		m_dwTotal = dwTotal;
	}

	void CFrameUI::SetFrameImage(LPCTSTR lpszFrameImg)
	{
		if (m_strFrameImg.CompareNoCase(lpszFrameImg) == 0)
			return;

		m_dwFrame = 0;
		m_strFrameImg = lpszFrameImg;

		ZeroMemory(&m_ptTopLeft, sizeof(m_ptTopLeft));

		Invalidate();
	}

	void CFrameUI::Run()
	{
		if (m_pManager == NULL)
			return;
		m_pManager->KillTimer(this, FRAME_ANIMATE_TIME_ID);	
		m_pManager->SetTimer(this, FRAME_ANIMATE_TIME_ID, m_dwPeroid/m_dwTotal);
	}
}