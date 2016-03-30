#include "stdafx.h"
#include "RollTextUI.h"

namespace DuiLib
{
	CRollTextUI::CRollTextUI()
	{
		m_nScrollXPos = 50;
		m_nScrollYPos = 50;
		m_nStep = 3;
		m_bRollText = FALSE;
		m_nRollDirection=ROLLTEXT_DIRECTION_LEFT;
		m_bTextChanged = FALSE;
		m_bTextPosed = FALSE;

		m_nTextHeight = 0;
		m_nTextWidth = 0;
	}

	CRollTextUI::~CRollTextUI()
	{
		m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
	}

	LPCTSTR CRollTextUI::GetClass() const
	{
		return _T("RollTextUI");
	}

	LPVOID CRollTextUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_ROLLTEXT) == 0 ) return static_cast<CRollTextUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	void CRollTextUI::DoInit()
	{
		BeginRoll();
	}

	void CRollTextUI::BeginRoll()
	{		
		m_pManager->SetTimer(this, ROLLTEXT_TIMERID, ROLLTEXT_TIMERID_SPAN);

		m_bRollText = TRUE;
	}

	void CRollTextUI::EndRoll()
	{
		if (m_bRollText == FALSE)
			return;

		m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
		m_bRollText = FALSE;
	}

	void CRollTextUI::SetPos(RECT rc)
	{
		m_bTextPosed = TRUE;

		CLabelUI::SetPos(rc);
	}

	void CRollTextUI::SetText(LPCTSTR pstrText)
	{
		m_bTextChanged = TRUE;

		CLabelUI::SetText(pstrText);
	}

	void CRollTextUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("rolltext")) == 0 ) SetRollText(_tcsicmp(pstrValue, _T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("rolltype")) ==0 )  SetRollType(pstrValue);
		else if (_tcsicmp(pstrName, _T("step")) == 0 )  SetRollTextStep(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("overstop")) == 0 ) SetOverStop(_tcsicmp(pstrValue, _T("true")) ==0);
		else
			CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CRollTextUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_TIMER && event.wParam == ROLLTEXT_TIMERID ) 
		{
			CDuiRect  rcClient;
			rcClient = m_rcItem;

			switch(m_nRollDirection)
			{
			case ROLLTEXT_DIRECTION_LEFT://向左动
				{
					if(m_nScrollXPos <= rcClient.left - m_nTextWidth)
						m_nScrollXPos = rcClient.right;
					else
						m_nScrollXPos -= m_nStep;
				
					break;
				}
			case ROLLTEXT_DIRECTION_RIGHT://向右动
				{
					if(m_nScrollXPos >= rcClient.right)
						m_nScrollXPos = rcClient.left - m_nTextWidth;
					else
						m_nScrollXPos += m_nStep;
					break;
				}
			case ROLLTEXT_DIRECTION_UP://向上动
				{
					if(m_nScrollYPos <= rcClient.top - m_nTextHeight)
						m_nScrollYPos = rcClient.bottom;
					else
						m_nScrollYPos -= m_nStep;
					break;
				}
			case ROLLTEXT_DIRECTION_DOWN://向下动
				{
					if(m_nScrollYPos >= rcClient.bottom)
						m_nScrollYPos = rcClient.top - m_nTextHeight;
					else
						m_nScrollYPos += m_nStep;
					break;
				}
			}
			Invalidate();
			return;
		}
		else if (event.Type == UIEVENT_MOUSEENTER){
			if (m_bRollText && m_bOverStop)
				m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
		}
		else if (event.Type == UIEVENT_MOUSELEAVE){
			if (m_bRollText)
				m_pManager->SetTimer(this, ROLLTEXT_TIMERID, ROLLTEXT_TIMERID_SPAN);
		}
		else
			CLabelUI::DoEvent(event);
	}

	void CRollTextUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
		DWORD dwTextColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;

		CDuiString sText = GetText();
		if( sText.IsEmpty() ) return;

		CDuiRect  rcClient;
		rcClient = m_rcItem;

		RECT rcItem = m_rcItem;
		rcItem.left += m_rcTextPadding.left;
		rcItem.right -= m_rcTextPadding.right;
		rcItem.top += m_rcTextPadding.top;
		rcItem.bottom -= m_rcTextPadding.bottom;

		BOOL bUpDown = m_nRollDirection == ROLLTEXT_DIRECTION_DOWN || 
										m_nRollDirection == ROLLTEXT_DIRECTION_UP;

		if (m_bTextChanged == TRUE)
		{
			m_nTextHeight = 0;
			if (bUpDown)
			{
				//当包含多端文字时，需要挨个显示
				//std::vector<CDuiString> vStrs = StrSplit(sText, _T("\n"));
				//for(int i = 0; i < vStrs.size(); i++) {
				SIZE sz = { 0 };
				::GetTextExtentPoint32(hDC, sText, sText.GetLength(), &sz);
				if(sz.cx > m_nTextWidth) m_nTextWidth = sz.cx;
				m_nTextHeight += sz.cy;
				//}
			}
			else
			{
				SIZE sz = { 0 };
				::GetTextExtentPoint32(hDC, sText, sText.GetLength(), &sz);
				if(sz.cx > m_nTextWidth) m_nTextWidth = sz.cx;
			}
			m_bTextChanged = FALSE;
		}
		
		int nXPos = 0, nYPos = 0;
		UINT uTextStyle =  DT_EDITCONTROL|DT_SINGLELINE;
		if(bUpDown) {
			nXPos = rcClient.left + (rcClient.GetWidth() - m_nTextWidth)/2;
			nYPos = rcClient.top;
			if(m_bTextPosed == TRUE)
			{
				m_nScrollXPos = nXPos;
				m_nScrollYPos = nYPos;
				m_bTextPosed = FALSE;
			}

			if(m_bRollText) nYPos = m_nScrollYPos;

			rcItem.left = nXPos;
			rcItem.top = nYPos;
			rcItem.bottom = rcItem.top + m_nTextHeight;
		}
		else {
			nXPos = rcClient.left;
			nYPos = rcClient.top + (rcClient.GetHeight() - m_nTextHeight)/2;
			if(m_bTextPosed == TRUE)
			{
				m_nScrollXPos = nXPos;
				m_nScrollYPos = nYPos;
				m_bTextPosed = FALSE;
			}

			if(m_bRollText) nXPos = m_nScrollXPos;

			rcItem.left = nXPos;
			rcItem.top = nYPos;
			rcItem.right = rcItem.left + m_nTextWidth;
			if (rcItem.right > m_rcItem.right)
				rcItem.right = m_rcItem.right;
		}

		int nLink = 0;
		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, sText, dwTextColor, NULL, NULL, nLink, uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rcItem, sText, dwTextColor, m_iFont, uTextStyle);
	}

	void CRollTextUI::SetRollText(BOOL bRollText)
	{
		if (m_bRollText == bRollText)
			return;

		m_bRollText = bRollText;
	}

	void CRollTextUI::SetRollType(LPCTSTR lpszRollType)
	{
		ROLLTEXT_DIRECTION nRollType = ROLLTEXT_DIRECTION_LEFT;
		if (_tcsicmp(lpszRollType, _T("left")) == 0)	nRollType = ROLLTEXT_DIRECTION_LEFT;
		else if (_tcsicmp(lpszRollType, _T("right")) ==0 )	nRollType = ROLLTEXT_DIRECTION_RIGHT;
		else if (_tcsicmp(lpszRollType, _T("up")) == 0 ) nRollType = ROLLTEXT_DIRECTION_UP;
		else if (_tcsicmp(lpszRollType, _T("down")) == 0 ) nRollType = ROLLTEXT_DIRECTION_DOWN;

		m_nRollDirection = nRollType;
	}

	void CRollTextUI::SetRollTextStep(int nStep)
	{
		if (nStep == m_nStep)
			return;

		m_nStep = nStep;
	}

	void CRollTextUI::SetOverStop(BOOL bOverStop)
	{
		if (m_bOverStop = bOverStop)
			return;

		m_bOverStop = bOverStop;
	}
}