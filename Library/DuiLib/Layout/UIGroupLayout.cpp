#include "stdafx.h"
#include "UIGroupLayout.h"

namespace DuiLib
{

	#define	GROUPBOX_ROUNDRECT_OFFSET_X		5
	#define   GROUPBOX_ROUNDRECT_OFFSET_Y		10
	#define	GROUPBOX_TEXT_OFFSET						20

	CGroupLayoutUI::CGroupLayoutUI()
		:m_dwTextColor(0xFF000000)
		,m_hBackground(NULL)
	{
		m_nBorderSize = 1;
		m_dwBorderColor = 0xFFCDCDCD;
		m_cxyBorderRound.cx = 5;
		m_cxyBorderRound.cy = 5;
		
		m_rcInset.left = GROUPBOX_ROUNDRECT_OFFSET_X;
		m_rcInset.top = GROUPBOX_ROUNDRECT_OFFSET_Y*2;
		m_rcInset.right = GROUPBOX_ROUNDRECT_OFFSET_X;
		m_rcInset.bottom = GROUPBOX_ROUNDRECT_OFFSET_Y;

		ZeroMemory(&m_rcTextArea,sizeof(RECT));
	}

	CGroupLayoutUI::~CGroupLayoutUI()
	{
		if (m_hBitMap != NULL)
			DeleteObject(m_hBitMap);
		if (m_hBackground != NULL)
			DeleteDC(m_hBackground);
	}

	LPCTSTR CGroupLayoutUI::GetClass() const
	{
		return _T("CGroupLayoutUI");
	}

	LPVOID CGroupLayoutUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_GROUPLAYOUT) == 0 ) return static_cast<CGroupLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}
	
	void CGroupLayoutUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;
		// Adjust for inset
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

 		if( m_items.GetSize() == 0) {
 			ProcessScrollBar(rc, 0, 0);
 			return;
 		}
		//////////////////////////////////////////////////////////////////////////
		///added by gechunping on 2014_04_02 for process border
 		if(m_rcBorderSize.left > 0 || m_rcBorderSize.top > 0 || m_rcBorderSize.right > 0 || m_rcBorderSize.bottom > 0){
 			rc.left += m_rcBorderSize.left;
 			rc.top += m_rcBorderSize.top;
 			rc.right -= m_rcBorderSize.right;
 			rc.bottom -= m_rcBorderSize.bottom;
 		}
 		else if(m_nBorderSize>0){
 			rc.left += m_nBorderSize;
 			rc.top +=m_nBorderSize;
 			rc.right -= m_nBorderSize;
 			rc.bottom -= m_nBorderSize;
 		}
		//////////////////////////////////////////////////////////////////////////
		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
			szAvailable.cy += m_pVerticalScrollBar->GetScrollRange();

		int iPosX = rc.left;
		int iPosY = rc.top;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			iPosY -= m_pVerticalScrollBar->GetScrollPos();
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		}
		int cyNeeded = 0;
		int cxNeeded = 0;
		for (int it1 = 0; it1 < m_items.GetSize(); ++it1)
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat())
			{
				SetFloatPos(it1);
				continue;
			}
			RECT rcPadding = pControl->GetPadding();
			SIZE sz = pControl->EstimateSize(szAvailable);
			SIZE rcPos = pControl->GetFixedXY();
			RECT rcCtrl = {iPosX + rcPadding.left + rcPos.cx,iPosY + rcPadding.top + rcPos.cy,
									iPosX + rcPadding.right + sz.cx+rcPos.cx,iPosY + rcPadding.bottom + sz.cy+rcPos.cy};

			pControl->SetPos(rcCtrl);
		}
	}

	void CGroupLayoutUI::PaintText(HDC hDC)
	{
		RECT rc = m_rcItem;
		rc.left += GROUPBOX_TEXT_OFFSET;
		rc.right += GROUPBOX_TEXT_OFFSET;

		RECT rcLine = m_rcItem;
		rcLine.left += GROUPBOX_TEXT_OFFSET;
		rcLine.top += GROUPBOX_ROUNDRECT_OFFSET_Y;
		rcLine.right = rcLine.left + m_rcTextArea.right-m_rcTextArea.left;
		rcLine.bottom = rcLine.top;

		BitBlt(hDC,rcLine.left,rcLine.top,m_rcTextArea.right-m_rcTextArea.left,m_rcTextArea.bottom-m_rcTextArea.top,m_hBackground,0,0,SRCCOPY);
		CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor,0, DT_SINGLELINE);
	}

	void CGroupLayoutUI::PaintStatusImage(HDC hDC)
	{
		RECT rc = m_rcItem;
		rc.left += GROUPBOX_ROUNDRECT_OFFSET_X;
		rc.right -= GROUPBOX_ROUNDRECT_OFFSET_X;
		rc.top += GROUPBOX_ROUNDRECT_OFFSET_Y;
		rc.bottom -= GROUPBOX_ROUNDRECT_OFFSET_Y;
		
		if (m_hBackground == NULL)
		{
			DWORD dwTextLen = _tcslen(m_sText);
			CRenderEngine::DrawText(hDC, m_pManager, m_rcTextArea, m_sText, m_dwTextColor,0, DT_CALCRECT);

			m_hBackground = CreateCompatibleDC(hDC);
			UINT nWidth = m_rcTextArea.right-m_rcTextArea.left ;
			UINT nHeight = m_rcTextArea.bottom-m_rcTextArea.top;
			m_hBitMap = CreateCompatibleBitmap(hDC,nWidth,nHeight);
			SelectObject(m_hBackground,m_hBitMap);
			
			BitBlt(m_hBackground,m_rcTextArea.left,m_rcTextArea.top,nWidth,nHeight,hDC,rc.left,rc.top,SRCCOPY);
		}		

 		if (m_strGroupBoxRoundRectImage.IsEmpty())
 			CRenderEngine::DrawRoundRect(hDC,rc,m_nBorderSize,m_cxyBorderRound.cx,m_cxyBorderRound.cy,m_dwBorderColor);
		else
			DrawImage(hDC,m_strGroupBoxRoundRectImage);
	}

	void CGroupLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName,_T("roundimage")) == 0)	SetGroupRoundImage(pstrValue);
		else
			CContainerUI::SetAttribute(pstrName,pstrValue);
	}

	void CGroupLayoutUI::PaintBorder(HDC hDC)
	{

	}

	void CGroupLayoutUI::SetGroupRoundImage(LPCTSTR pstrValue)
	{
		if (m_strGroupBoxRoundRectImage == pstrValue)
			return;
		m_strGroupBoxRoundRectImage = pstrValue;
		Invalidate();
	}
}