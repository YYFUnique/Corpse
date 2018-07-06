#include "StdAfx.h"
#include "UIRichText.h"
#include "DllCore/Utils/TextTools.h"
namespace DuiLib
{
	CRichTextUI::CRichTextUI()
	{
		m_dwParaSpace = 0;
	}

	CRichTextUI::~CRichTextUI()
	{

	}

	LPVOID CRichTextUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_RICHTEXT) == 0) return static_cast<CRichTextUI*>(this);
		else
			return CRichEditUI::GetInterface(pstrName);
	}

	void CRichTextUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		bool bVScrollBarVisiable = false;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			bVScrollBarVisiable = true;
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}

		if( m_pTwh ) {
			RECT rcRich = rc;
			rcRich.left += m_rcTextPadding.left;
			rcRich.right -= m_rcTextPadding.right;
			rcRich.top += m_rcTextPadding.top;
			rcRich.bottom -= m_rcTextPadding.bottom;
			
			LPRECT pClient = m_pTwh->GetClientRect();
			if (EqualRect(pClient, &rcRich) == FALSE)
				m_pTwh->SetClientRect(&rcRich);

			if( bVScrollBarVisiable && (!m_pVerticalScrollBar->IsVisible() || m_bVScrollBarFixing) ) {
				LONG lWidth = rcRich.right - rcRich.left + m_pVerticalScrollBar->GetFixedWidth();
				LONG lHeight = 0;
				SIZEL szExtent = { -1, -1 };
				m_pTwh->GetTextServices()->TxGetNaturalSize(
					DVASPECT_CONTENT, 
					GetManager()->GetPaintDC(), 
					NULL,
					NULL,
					TXTNS_FITTOCONTENT,
					&szExtent,
					&lWidth,
					&lHeight);
				if( lHeight > rcRich.bottom - rcRich.top ) {
					m_pVerticalScrollBar->SetVisible(true);
					m_pVerticalScrollBar->SetScrollPos(0);
					m_bVScrollBarFixing = true;
				}
				else {
					if( m_bVScrollBarFixing ) {
						m_pVerticalScrollBar->SetVisible(false);
						m_bVScrollBarFixing = false;
					}
				}
			}
		}

		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
			RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom};
			m_pVerticalScrollBar->SetPos(rcScrollBarPos);
		}
		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
			RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
			m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
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

		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cy == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		int cxNeeded = 0;
		if( nAdjustables > 0 ) cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		int iPosX = rc.left;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			iPosY -= m_pVerticalScrollBar->GetScrollPos();
		}

		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		}
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cy -= rcPadding.top;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if( sz.cy == 0 ) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				//////////////////////////////////////////////////////////////////////////
				///corrected by gechunping  on 2014_3_27 
				///deleted origin 
				// 				if( iAdjustable == nAdjustables ) {
				// 					sz.cy = MAX(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				// 				} 
				///corrected by gechunping  on 2014_3_27
				//////////////////////////////////////////////////////////////////////////

				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = pControl->GetFixedWidth();
			if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if( sz.cx < 0 ) sz.cx = 0;
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			int tmp = sz.cx + rcPadding.left + rcPadding.right;
			cxNeeded = (tmp > cxNeeded) ? tmp: cxNeeded;

			//////////////////////////////////////////////////////////////////////////
			///corrected by gechunping  on 2014_3_27
			///origin///  RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy +  rcPadding.bottom };
			///corrected by gechunping  on 2014_3_27
			//////////////////////////////////////////////////////////////////////////
			pControl->SetPos(rcCtrl);

			/*CDuiString strTipInfo;
			strTipInfo.Format(_T("IPosY:%d,left:%d,top:%d,right:%d,bottom:%d"),iPosY,rcCtrl.left,rcCtrl.top,rcCtrl.right,rcCtrl.bottom);
			OutputDebugString(strTipInfo);*/

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;

		//ProcessScrollBar(rc, cxNeeded, cyNeeded+15);
	}

	/*void CRichTextUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}
		else 
			CRichEditUI::DoEvent(event);
	}*/

	void CRichTextUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

		CRenderClip clip;
		CRenderClip::GenerateClip(hDC, rcTemp, clip);
		CControlUI::DoPaint(hDC, rcPaint);

		if( m_items.GetSize() > 0 ) {
			RECT rc = m_rcItem;
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

			if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
			else {
				CRenderClip childClip;
				CRenderClip::GenerateClip(hDC, rcTemp, childClip);
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						CRenderClip::UseOldClipBegin(hDC, childClip);
						pControl->DoPaint(hDC, rcPaint);
						CRenderClip::UseOldClipEnd(hDC, childClip);
					}
					else {
						if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
		}

		if( m_pTwh ) {
			RECT rc;
			m_pTwh->GetControlRect(&rc);
			// Remember wparam is actually the hdc and lparam is the update
			// rect because this message has been preprocessed by the window.
			m_pTwh->GetTextServices()->TxDraw(
				DVASPECT_CONTENT,  		// Draw Aspect
				/*-1*/0,				// Lindex
				NULL,					// Info for drawing optimazation
				NULL,					// target device information
				hDC,			        // Draw device HDC
				NULL, 				   	// Target device HDC
				(RECTL*)&rc,			// Bounding client rectangle
				NULL, 		            // Clipping rectangle for metafiles
				(RECT*)&rcPaint,		// Update rectangle
				NULL, 	   				// Call back function
				NULL,					// Call back parameter
				0);				        // What view of the object
			if( m_bVScrollBarFixing ) {
				LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
				LONG lHeight = 0;
				SIZEL szExtent = { -1, -1 };
				m_pTwh->GetTextServices()->TxGetNaturalSize(
					DVASPECT_CONTENT, 
					GetManager()->GetPaintDC(), 
					NULL,
					NULL,
					TXTNS_FITTOCONTENT,
					&szExtent,
					&lWidth,
					&lHeight);
				if( lHeight <= rc.bottom - rc.top ) {
					NeedUpdate();
				}
			}
		}

		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
				m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
			}
		}

		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
				m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
			}
		}

		CHARRANGE chRange = {0};
		bool bShow = true;
		if (GetSel(chRange) != false)
			bShow = chRange.cpMin == chRange.cpMax ? true : false;

		if (bShow && GetManager()->IsLayered() && m_bShowCaret && m_bFocused)
			CRenderEngine::DrawColor(hDC,m_rcPos,m_dwCaretColor);

		PaintBorder(hDC);
	}

	void CRichTextUI::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			if (iLastScrollPos != szPos.cy) {
				m_pVerticalScrollBar->SetScrollPos(szPos.cy);
				cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
			}
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			if (iLastScrollPos != szPos.cx) {
				m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
				cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
			}  
		}
		if( cy != 0 ) {
			int iPos = 0;
			if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
				iPos = m_pVerticalScrollBar->GetScrollPos();
			WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pVerticalScrollBar->GetScrollPos());
			TxSendMessage(WM_VSCROLL, wParam, 0L, 0);
			if( m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
				if( cy > 0 && m_pVerticalScrollBar->GetScrollPos() <= iPos )
					m_pVerticalScrollBar->SetScrollPos(iPos);
			}

			RECT rcPos;
			for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
				if( !pControl->IsVisible() ) continue;
				if( pControl->IsFloat() ) continue;

				rcPos = pControl->GetPos();
				rcPos.top -= cy;
				rcPos.bottom -= cy;
				pControl->SetPos(rcPos);
			}			
		}
		if( cx != 0 ) {
			WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pHorizontalScrollBar->GetScrollPos());
			TxSendMessage(WM_HSCROLL, wParam, 0L, 0);
		}
	}

	// 支持鼠标滑动时，控件滚动
	void CRichTextUI::LineUp()
	{
		CContainerUI::LineUp();
	}

	// 支持鼠标滑动时，控件滚动
	void CRichTextUI::LineDown()
	{
		CContainerUI::LineDown();
	}

	void CRichTextUI::PageUp()
	{
		CContainerUI::PageUp();
	}

	void CRichTextUI::PageDown()
	{
		CContainerUI::PageDown();
	}

	void CRichTextUI::HomeUp()
	{
		CContainerUI::HomeUp();
	}

	void CRichTextUI::EndDown()
	{
		//TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
		CContainerUI::EndDown();
	}

	void CRichTextUI::LineLeft()
	{
		CContainerUI::LineLeft();
	}

	void CRichTextUI::LineRight()
	{
		CContainerUI::LineRight();
	}

	void CRichTextUI::PageLeft()
	{
		CContainerUI::PageLeft();
	}

	void CRichTextUI::PageRight()
	{
		CContainerUI::PageRight();
	}

	void CRichTextUI::HomeLeft()
	{
		CContainerUI::HomeLeft();
	}

	void CRichTextUI::EndRight()
	{
		CContainerUI::EndRight();
	}

	DWORD CRichTextUI::GetDefaultCharFormat(CHARFORMAT &cf) const
	{
		cf.cbSize = sizeof(CHARFORMAT);
		HRESULT lRet = 0;
		GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lRet);
		return (DWORD)lRet;
	}

	BOOL CRichTextUI::SetDefaultCharFormat(CHARFORMAT &cf)
	{
		cf.cbSize = sizeof(CHARFORMAT);
		HRESULT lRet = 0;
		GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lRet);
		return (BOOL)lRet;
	}

	void CRichTextUI::SetDefaultFont(LPCTSTR lpFontName,int nFontSize, COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink)
	{
		CHARFORMAT cf = {0};
		GetDefaultCharFormat(cf);

		if (lpFontName != NULL)									// 设置字体名称
		{
			cf.dwMask |= CFM_FACE;
			_tcscpy_s(cf.szFaceName, lpFontName);
		}

		if (nFontSize > 0)												//	设置字体高度
		{
			cf.dwMask |= CFM_SIZE;
			cf.yHeight = nFontSize * 20;
		}

		cf.dwMask |= CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_LINK;

		cf.crTextColor = clrText;										//	设置字体颜色
		cf.dwEffects &= ~CFE_AUTOCOLOR;					

		if (bBold)																//	设置字体粗体
			cf.dwEffects |= CFE_BOLD;
		else
			cf.dwEffects |= ~CFE_BOLD;

		if (bItalic)
			cf.dwEffects |= CFE_ITALIC;								// 设置字体倾斜
		else
			cf.dwEffects |= ~CFE_ITALIC;

		if (bUnderLine)													// 设置下划线
			cf.dwEffects |= CFE_UNDERLINE;
		else
			cf.dwEffects |= ~CFE_UNDERLINE;

		if (bIsLink)
			cf.dwEffects |= CFE_LINK;								//	设置超链接
		else
			cf.dwEffects |= ~CFE_LINK;

		SetDefaultCharFormat(cf);
	}

	//void CRichTextUI::AddBubble(CBubbleInfo* sbi)
	//{
	//	CBubbleInfo* pBubbleInfo = new CBubbleInfo;
	//	pBubbleInfo->m_dwStart = sbi->m_dwStart;
	//	pBubbleInfo->m_dwEnd = sbi->m_dwEnd;
	//	pBubbleInfo->m_pBubble = sbi->m_pBubble;
	//	//pBubbleInfo->m_pBubble = pBubble;
	//	Add(pBubbleInfo->m_pBubble);
	//}

	/*void CRichTextUI::RejustBubble(CBubbleInfo& sbi)
	{
		CalAsLeft(sbi);
	}*/

	DWORD CRichTextUI::GetParaSpace() const
	{
		return m_dwParaSpace;
	}

	void CRichTextUI::SetParaSpace(DWORD dwParaSpace)
	{
		m_dwParaSpace = dwParaSpace;
	}

	int CRichTextUI::GetWindowTextLength() const
	{
		HRESULT lRet = 0;
		GetTextServices()->TxSendMessage(WM_GETTEXTLENGTH, 0, 0, &lRet);
		return (int)lRet;
	}

	BOOL CRichTextUI::GetTextRange(DWORD dwStart, DWORD dwEnd, CString& strRangeText) const
	{
		CHARRANGE chRange = {dwStart, dwEnd};
		return GetTextRange(&chRange, strRangeText);
	}

	BOOL CRichTextUI::GetTextRange(CHARRANGE* pChrg, CString& strRangeText) const
	{
		strRangeText.Empty();
		BOOL bSuccess = FALSE;

		LPTSTR lpText = NULL;
		do 
		{
			if (pChrg == NULL || pChrg->cpMax <= pChrg->cpMin)
				break;

			LONG nLen = pChrg->cpMax - pChrg->cpMin;
			lpText = new TCHAR[nLen + 1];
			if (lpText == NULL)
				break;

			TEXTRANGE TextRange = {0};
			TextRange.chrg = *pChrg;
			TextRange.lpstrText = lpText;

			HRESULT lRet = FALSE;
			GetTextServices()->TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&TextRange, &lRet);

			strRangeText = lpText;
			bSuccess = (BOOL)lRet;
		} while (FALSE);

		if (lpText != NULL)
		{
			delete[] lpText;
			lpText = NULL;
		}

		return bSuccess;
	}

	void CRichTextUI::ReplaceSel(LPCTSTR lpszNewText, LPCTSTR lpFontName, int nFontSize,	COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink, int nStartIndent, BOOL bCanUndo/* = FALSE*/)
	{
		LONG lStartChar = 0, lEndChar = 0;
		GetSel(lStartChar, lEndChar);
		CRichEditUI::ReplaceSel(lpszNewText, bCanUndo != FALSE);

		lEndChar = lStartChar + _tcslen(lpszNewText);
		SetSel(lStartChar, lEndChar);
		SetFont(lpFontName, nFontSize, clrText, bBold, bItalic, bUnderLine, bIsLink);
		SetSel(lEndChar, lEndChar);
	}

	BOOL CRichTextUI::SetParaSpace(DWORD dwBefore, DWORD dwAfter)
	{
		PARAFORMAT2 pf;
		ZeroMemory(&pf, sizeof(PARAFORMAT2));

		pf.cbSize						= sizeof(PARAFORMAT2);
		if (dwBefore != 0)
		{
			pf.dwMask			 |= PFM_SPACEBEFORE;
			pf.dySpaceBefore = 15 * dwBefore;
		}
		if (dwAfter != 0)
		{
			pf.dwMask |= PFM_SPACEAFTER;
			pf.dySpaceAfter = 15 * dwAfter;
		}

		HRESULT lRes = 0;
		GetTextServices()->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
		return (BOOL)lRes;
	}

	// 设置左缩进(单位:缇)
	BOOL CRichTextUI::SetStartIndent(int nLeftSize, int nRightSize, DWORD dwAlignment /*= PFA_LEFT*/)
	{
		PARAFORMAT2 pf;
		pf.cbSize        = sizeof(PARAFORMAT2);
		pf.dwMask        = /*PFM_OFFSET*/ /*|*/ PFM_STARTINDENT | PFM_RIGHTINDENT | PFM_ALIGNMENT;
		pf.wAlignment = PFA_LEFT;
		//pf.dxOffset      = 0;		// 用于控制显示消息内容(相对于首行缩进正便宜)
		pf.dxStartIndent = 15*nLeftSize;			// 首行缩进
		pf.dxRightIndent = 15*nRightSize;		// 右缩进
		
		HRESULT lRes = 0;
		GetTextServices()->TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lRes);
		return (BOOL)lRes;
	}

	/*DWORD CRichTextUI::GetSelectionCharFormat(CHARFORMAT& cf) const
	{
		cf.cbSize = sizeof(CHARFORMAT);
		LRESULT lRes = 0;
		GetTextServices()->TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lRes);
		return (DWORD)lRes;
	}

	BOOL CRichTextUI::SetSelectionCharFormat(CHARFORMAT &cf)
	{
		cf.cbSize = sizeof(CHARFORMAT);
		LRESULT lRes = 0;
		GetTextServices()->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lRes);
		return (BOOL)lRes;
	}*/

	DWORD CRichTextUI::SetFont(LPCTSTR lpFontName, int nFontSize, COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink)
	{
		//CHARFORMAT cf;
		//ZeroMemory(&cf, sizeof(cf));

		//cf.cbSize = sizeof(CHARFORMAT);
		////GetSelectionCharFormat(cf);

		//if (lpFontName != NULL)									// 设置字体名称
		//{
		//	cf.dwMask |= CFM_FACE;
		//	_tcscpy_s(cf.szFaceName, _countof(cf.szFaceName), lpFontName);
		//}

		//if (nFontSize > 0)												//	设置字体高度
		//{
		//	cf.dwMask |= CFM_SIZE;
		//	cf.yHeight = nFontSize * 20;
		//}

		////cf.dwMask |= CFM_COLOR | CFM_LINK;

		//cf.crTextColor = clrText;										//	设置字体颜色
		////cf.dwMask |= CFM_COLOR;
		////cf.dwEffects &= ~CFE_AUTOCOLOR;
		//cf.dwEffects &= ~CFE_AUTOCOLOR;

		//if (bBold) {
		//	cf.dwEffects |= CFE_BOLD;								//	设置字体粗体
		//	cf.dwMask |= CFM_BOLD;
		//} else {
		//	cf.dwEffects &= ~CFE_BOLD;
		//	cf.dwMask &= ~CFM_BOLD;
		//}

		//if (bItalic) {
		//	cf.dwEffects |= CFE_ITALIC;								// 设置字体倾斜
		//	cf.dwMask |= CFM_ITALIC;
		//} else {
		//	cf.dwEffects &= ~CFE_ITALIC;
		//	cf.dwMask &= ~CFM_ITALIC;
		//}

		//if (bUnderLine) {
		//	cf.dwEffects |= CFE_UNDERLINE;
		//	cf.dwMask |= CFM_UNDERLINE;
		//} else {
		//	cf.dwEffects &= ~CFE_UNDERLINE;
		//	cf.dwMask &= ~CFM_UNDERLINE;
		//}

		//if (bIsLink) {
		//	cf.dwEffects |= CFE_LINK;
		//	cf.dwMask |= CFM_LINK;
		//} else {
		//	cf.dwEffects &= ~CFE_LINK;
		//	cf.dwMask &= ~CFM_ITALIC;
		//}

		//return SetSelectionCharFormat(cf);
		return TRUE;
	}

	/*void CRichTextUI::EnsureVisible()
	{
		CHARRANGE CharRange;
		GetSel(CharRange);

		RECT rcListInset = m_pList->GetInset();

		rcList.left += rcListInset.left;
		rcList.top += rcListInset.top;
		rcList.right -= rcListInset.right;
		rcList.bottom -= rcListInset.bottom;

		CScrollBarUI* pHorizontalScrollBar = m_pHorizontalScrollBar->GetHorizontalScrollBar();
		if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

		int iPos = m_pList->GetScrollPos().cy;
		if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
		int dx = 0;
		if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
		if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
		Scroll(0, dx);
	}*/

	//void CRichTextUI::CalAsLeft(CBubbleInfo &sbi)
	//{
	//	// 计算项目应有的宽和高
	//	int iHeight = 0;
	//	int iWidth = 0;

	//	// 计算文字高度
	//	POINT ps = PosFromChar(sbi.m_dwStart);
	//	POINT pe = PosFromChar(sbi.m_dwEnd);

	//	iHeight = pe.y - ps.y;

	//	// 计算文字宽度
	//	LONG ls = LineFromChar(sbi.m_dwStart);
	//	LONG le = LineFromChar(sbi.m_dwEnd);

	//	if (ls != le) {
	//		// 如果是多行文本 ,计算第一个字符x坐标
	//		SetSel(sbi.m_dwStart, sbi.m_dwEnd);
	//		int iLeft = GetCurTargetX();
	//		for (int i=ls;i<le;++i)
	//		{
	//			int iIndex = LineIndex(i);
	//			int iLineLen = LineLength(iIndex);
	//			if (iLineLen == 0)
	//				continue;
	//			int iLast = iIndex + iLineLen;
	//			SetSel(iLast, iLast);
	//			int iRight = GetCurTargetX();
	//			if (iRight <= iLeft)
	//			{
	//				RECT rc;
	//				m_pTwh->GetControlRect(&rc);
	//				int nBar = 0;
	//				if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
	//					nBar = m_pVerticalScrollBar->GetFixedHeight();
	//				iWidth = rc.right - rc.left - nBar - 32;
	//				//break;
	//			} else {
	//				iWidth = max(iRight - 5, iWidth);
	//			}
	//		}
	//	} else {
	//		//iWidth = MAX();
	//	}

	//	SIZE size = {iWidth, iHeight};
	//	sbi.m_pBubble->SetBubbleSize(size);
	//}

	LONG CRichTextUI::GetCurTargetX() const
	{
		LONG lTargetX = 0;
		GetTextServices()->TxGetCurTargetX(&lTargetX);

		return lTargetX;
	}

	void CRichTextUI::GetControlRect(LPRECT lpControlRect) const
	{
		m_pTwh->GetControlRect(lpControlRect);
	}
}
