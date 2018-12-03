#include "StdAfx.h"
#include "EditTextHost.h"

namespace DuiLib
{
	#define    IME_BLINK_TIMER_ID			1000
	CEditTextHost::CEditTextHost()
		: m_pOwner(NULL)
		, m_bCaretOn(FALSE)
		, m_dwTextColor(0)
		, m_dwDisabledTextColor(0)
		, m_dwSelectTextColor(0xFFFFFFFF)
		, m_dwSelectbkColor(0xFF3399FF)
		, m_dwWaterColor(0xFFBAC0C5)
		, m_bReadOnly(FALSE)
		, m_bPasswordMode(FALSE)
		, m_cPasswordChar(_T('*'))
		, m_bDigitalMode(FALSE)
		, m_bWaterMode(FALSE)
		, m_nLimitLength(-1)
		, m_nSelStart(0)
		, m_nCaretPos(0)
		, m_nCaretWidth(1)
		, m_iFont(-1)
		, m_nCaretOffset(0)
		, m_bFocused(FALSE)
		, m_nTextHostState(0)
		, m_bKeyDown(FALSE)
		, CalcCaretType(CALC_CARET_TYPE_NONE)
	{
		SetRect(&m_rcTextPadding, 4, 4, 4, 4);
		ZeroMemory(&m_rcCaret, sizeof(RECT));
		ZeroMemory(&m_szCaretPt, sizeof(POINT));
		m_uTextStyle = DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX;
	}

	CEditTextHost::~CEditTextHost()
	{

	}

	void CEditTextHost::SetOwner(CControlUI* pOwner)
	{
		if (pOwner == NULL)
			return;

		m_pOwner = pOwner;
	}

	CDuiString CEditTextHost::GetText() const
	{
		return m_sText;
	}

	void CEditTextHost::SetText(LPCTSTR pstrText)
	{
		if (m_sText == pstrText)
			return;

		m_strTextBak = pstrText;
		SetWaterMode(m_strTextBak.IsEmpty() == TRUE);
		
		CDuiString strText = GetEditText();
		m_nSelStart = m_nCaretPos = strText.GetLength();
		Invalidate();
	}

	BOOL CEditTextHost::DoEvent(TEventUI& event)
	{
		CPaintManagerUI* pManager = GetManager();
		HWND hWnd = pManager->GetPaintWindow();

		if (event.Type == UIEVENT_TIMER) {
			m_bCaretOn = !m_bCaretOn;
			InvalidateRect(hWnd ,&m_rcCaret, FALSE);
		} else if (event.Type == UIEVENT_SETFOCUS) {
			m_bFocused = TRUE;
			if (IsWaterMode())
				SetWaterMode(FALSE);
			
			pManager->SetTimer(m_pOwner, IME_BLINK_TIMER_ID, GetCaretBlinkTime());
			CreateCaret(hWnd, NULL, m_nCaretWidth, 2);
			Invalidate();
		} else if (event.Type == UIEVENT_KILLFOCUS) {
			m_nSelStart = m_nCaretPos;
			m_nCaretOffset = 0;
			m_bCaretOn = FALSE;
			m_bFocused = FALSE;
			if (m_strTextBak.IsEmpty() && m_strWaterText.IsEmpty() == FALSE)
				SetWaterMode(TRUE);
	
			pManager->KillTimer(m_pOwner, IME_BLINK_TIMER_ID);
			DestroyCaret();
			Invalidate();
		} else if( event.Type == UIEVENT_MOUSEENTER ) {
			if (m_pOwner->IsEnabled() ) {
				m_nTextHostState |= UISTATE_HOT;
				Invalidate();
			}
		} else if( event.Type == UIEVENT_MOUSELEAVE ) {
			if( IsEnabled() ) {
				m_nTextHostState &= ~UISTATE_HOT;
				Invalidate();
			}
		} else if (event.Type == UIEVENT_KEYDOWN){
			if (m_pOwner->IsEnabled()){
				if (IsReadOnly())
					return TRUE;

				if (OnKeyEvent(event) && (event.chKey < 'A' || event.chKey > 'Z'))
				{
					if ((event.wKeyState & MK_CONTROL) != MK_CONTROL &&
						(event.wKeyState & MK_SHIFT) != MK_SHIFT)
						m_nSelStart = m_nCaretPos;
					RestartCaretBlinkTimer();
				}
				Invalidate();
			}
		} else if (event.Type == UIEVENT_CHAR){
			if (event.chKey == VK_BACK || event.chKey == VK_RETURN || event.chKey == VK_ESCAPE || event.chKey == VK_TAB)
				return TRUE;
			if (event.wKeyState && (event.wKeyState & MK_SHIFT) != MK_SHIFT)
				return TRUE;

			if (IsEnabled()){
				if (IsReadOnly())
					return TRUE;

				if (IsDigitalMode() == TRUE && (event.chKey < 256 && isdigit(event.chKey)) == FALSE)
					return TRUE;

				int nMin = MIN(m_nSelStart,m_nCaretPos);
				int nMax = MAX(m_nSelStart,m_nCaretPos);
				CDuiString strLeft = m_sText.Left(nMin);
				CDuiString strRight = m_sText.Right(m_sText.GetLength()-nMax);
				CDuiString strValue;
				strValue.Format(_T("%s%c%s"),(LPCTSTR)strLeft,event.chKey,(LPCTSTR)strRight);
				if ((UINT)strValue.GetLength() <= GetLimitLength())
				{
					SetText(strValue);
					m_nSelStart = m_nCaretPos = nMin+1;
					RestartCaretBlinkTimer();
					GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
					Invalidate();
				}
			}
		} else if (event.Type == UIEVENT_MOUSEMOVE) {
			if ((m_nTextHostState & UISTATE_PUSHED) != UISTATE_PUSHED)
				return TRUE;
			//确定选择区域
			CalcCaretType = CALC_CARET_TYPE_SELECT_MODE;
			m_szCaretPt.x = event.ptMouse.x;
			m_szCaretPt.y = event.ptMouse.y;
			Invalidate();

		} else if (event.Type == UIEVENT_BUTTONDOWN) {
			if (m_pOwner->IsEnabled()){
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL || (event.wKeyState & MK_SHIFT) == MK_SHIFT)
					CalcCaretType = CALC_CARET_TYPE_SELECT_MODE;
				else
					CalcCaretType = CALC_CARET_TYPE_POS;
				m_nTextHostState |= UISTATE_PUSHED;
				m_szCaretPt.x = event.ptMouse.x;
				m_szCaretPt.y = event.ptMouse.y;
				RestartCaretBlinkTimer();
				Invalidate();
			}
		} else if (event.Type == UIEVENT_BUTTONUP){
			if (IsEnabled()){
				m_nTextHostState &= ~UISTATE_PUSHED;
				CalcCaretType = CALC_CARET_TYPE_NONE;
				Invalidate();
			}			
		} else if (event.Type == UIEVENT_IME_STARTCOMPOSITION) {
			// 调整输入法窗口位置
			AdjustImeWindowPos();
		} else
			return FALSE;

		return TRUE;
	}

	/************************************************************************/
	/* 根据文字计算文字应该占用的宽度                                                            */
	/************************************************************************/
	void CEditTextHost::CalcTextRect(HDC hDC,const CDuiString& strText,RECT& rcSelection,int nStart,int nLen)
	{
		ZeroMemory(&rcSelection,sizeof(RECT));
		CDuiString strSelectionText;
		if (nStart == 0)
			strSelectionText = strText.Left(nLen);
		else 
			strSelectionText = strText.Mid(nStart,nLen);

		CRenderEngine::DrawText(hDC, GetManager(), rcSelection, strSelectionText, 0, m_iFont,DT_CALCRECT|DT_NOPREFIX);

		//计算出的文字占用矩形
		UINT nWidth = rcSelection.right - rcSelection.left;
		UINT nHeight = rcSelection.bottom - rcSelection.top;

		//还需要除去选择文字前面所占空间
		if (nStart != 0)
		{
			RECT rcSelectionOffset = {0};
			strSelectionText = strText.Left(nStart);
			CRenderEngine::DrawText(hDC, GetManager(),rcSelectionOffset,strSelectionText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);

			rcSelection.left += rcSelectionOffset.right;
		}

		RECT rcItem = m_pOwner->GetPos();
		RECT rcTextPadding = GetTextPadding();

		rcSelection.left += rcItem.left + rcTextPadding.left;
		rcSelection.top += rcItem.top + rcTextPadding.top;
		rcSelection.right = rcSelection.left +nWidth;
		rcSelection.bottom = rcSelection.top + nHeight ;
	}

	void CEditTextHost::CalcCaretRect(HDC hDC, const CDuiString& sText)
	{
		static DWORD dwOffset = 0;

		RECT rcItem = m_pOwner->GetPos();
		RECT rcTextPadding = GetTextPadding();

		if (m_nCaretPos == 0 || sText.IsEmpty())
		{
			RECT rcText = rcItem;
			rcText.left += rcTextPadding.left;
			rcText.top += rcTextPadding.top;
			rcText.right -= rcTextPadding.right;
			rcText.bottom -= rcTextPadding.bottom;
			CRenderEngine::DrawText(hDC, GetManager(), rcText, sText, 0, m_iFont, m_uTextStyle|DT_CALCRECT);
			m_rcCaret.left = rcText.left;
			m_rcCaret.right = m_rcCaret.left + m_nCaretWidth;
			m_rcCaret.top = rcText.top;
			m_rcCaret.bottom = rcText.bottom;
		}
		else
		{
			CalcTextRect(hDC, sText, m_rcCaret, 0, m_nCaretPos);
			m_rcCaret.left = m_rcCaret.right;
			m_rcCaret.right += m_nCaretWidth;
		}

		//计算输入光标相对于编辑框的偏移量
		if (m_bKeyDown)
		{
			RECT rcTextRange = {0};
			CDuiString strDrawText = sText.Left(m_nCaretPos);
			CRenderEngine::DrawText(hDC,GetManager(),rcTextRange,strDrawText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);

			m_nCaretOffset = rcTextRange.right - rcTextRange.left - dwOffset;

			if (m_nCaretOffset > 0 )
			{
				CRenderEngine::DrawText(hDC,GetManager(),rcTextRange,sText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);
				LONG lTextRight = rcTextRange.right + rcItem.left + rcTextPadding.left - m_nCaretOffset;
				if (lTextRight < rcItem.right - rcTextPadding.right)
					m_nCaretOffset -= rcItem.right - rcTextPadding.right - lTextRight - m_nCaretWidth;
			}

			if (m_nCaretOffset <= 0)
				m_nCaretOffset = 0;
			FixedCaretPos(m_nCaretOffset);

			m_bKeyDown = FALSE;
		}
		else if (m_rcCaret.left <= rcItem.left + rcTextPadding.left + m_nCaretOffset)
		{
			m_nCaretOffset = m_rcCaret.left - rcTextPadding.left - rcItem.left;
			FixedCaretPos(m_nCaretOffset);
		}
		else if (m_rcCaret.right > rcItem.right - rcTextPadding.right && (rcItem.right  - rcTextPadding.right + m_nCaretOffset < m_rcCaret.right))
		{
			m_nCaretOffset = m_rcCaret.right - rcItem.right + rcTextPadding.right;
			FixedCaretPos(m_nCaretOffset);
		}
		else if (m_nCaretOffset)
		{
			FixedCaretPos(m_nCaretOffset);
		}

		dwOffset = m_rcCaret.left - rcItem.left - rcTextPadding.left;
	}

	int CEditTextHost::GetCaretPos(HDC hDC, const CDuiString& sText)
	{
		//鼠标在第几个文字后面
		int nCaretPos = 0;

		RECT rcCaretPos = {0};
		//测试鼠标点击命中位置，该算法是否有待优化
		for (int n=0;n<sText.GetLength();++n)
		{
			CalcTextRect(hDC,sText,rcCaretPos,n,1);
			int nWidth = rcCaretPos.right-rcCaretPos.left;
			rcCaretPos.left -= m_nCaretOffset;
			rcCaretPos.right = rcCaretPos.left + nWidth;
			//测试鼠标点击是命中目标文字
			if (PtInRect(&rcCaretPos, m_szCaretPt))
			{
				int nLeft = m_szCaretPt.x - rcCaretPos.left;
				int nRight = rcCaretPos.right - m_szCaretPt.x;
				int nMin = MIN(nLeft, nRight);
				int nPos = nMin == nLeft ? n : n+1;
				if (nPos > sText.GetLength())
					nCaretPos = sText.GetLength();
				else
					nCaretPos = nPos;
				break;
			}
		}

		if (rcCaretPos.right != rcCaretPos.left && rcCaretPos.top != rcCaretPos.bottom)
		{
			if (rcCaretPos.right <= m_szCaretPt.x)
				nCaretPos = sText.GetLength();
		}

		return nCaretPos;
	}

	void CEditTextHost::FixedCaretPos(int nFixed)
	{
		m_rcCaret.left -= nFixed;
		m_rcCaret.right = m_rcCaret.left + m_nCaretWidth;
	}

	void CEditTextHost::PaintCaret(HDC hDC)
	{
		if (m_pOwner && IsFocused() && m_bCaretOn)
		{
			CRenderEngine::DrawColor(hDC, m_rcCaret, m_dwCaretColor);
			SetCaretPos(m_rcCaret.left,m_rcCaret.top + (m_rcCaret.bottom-m_rcCaret.top)/2);

			AdjustImeWindowPos();
		}
	}

	void CEditTextHost::PaintText(HDC hDC)
	{
		if (m_dwTextColor == 0) m_dwTextColor = GetManager()->GetDefaultFontColor();
		if (m_dwDisabledTextColor == 0) m_dwDisabledTextColor = GetManager()->GetDefaultDisabledColor();

		CDuiString sText = GetEditText();

		RECT rcItem = m_pOwner->GetPos();
		RECT rcTextPadding = GetTextPadding();

		RECT rc = rcItem;
		rc.left += rcTextPadding.left;
		rc.right -= rcTextPadding.right;
		rc.top += rcTextPadding.top;
		rc.bottom -= rcTextPadding.bottom;

		if ((CalcCaretType & CALC_CARET_TYPE_POS) == CALC_CARET_TYPE_POS)
		{
			m_nCaretPos = GetCaretPos(hDC,sText);
			m_nSelStart = m_nCaretPos;
		}
		else if ((CalcCaretType & CALC_CARET_TYPE_SELECT_MODE) == CALC_CARET_TYPE_SELECT_MODE)
		{
			m_nCaretPos = GetCaretPos(hDC,sText);
		}		
		else if (CalcCaretType & CALC_CARET_TYPE_SELECT_ALL)
		{
			m_nSelStart = 0;
			m_nCaretPos = sText.GetLength();
		}

		CalcCaretType = CALC_CARET_TYPE_NONE;

		if (m_nTextHostState & UISTATE_FOCUSED)
			CalcCaretRect(hDC, sText);

		if (sText.IsEmpty()) return;

		DrawText(hDC,rc,sText);

		if (m_nCaretPos != m_nSelStart)
			DrawSelectionText(hDC, rc, sText);
	}

	void CEditTextHost::PaintStatusImage(HDC hDC)
	{
		if (IsFocused()) m_nTextHostState |= UISTATE_FOCUSED;
		else m_nTextHostState &= ~UISTATE_FOCUSED;

		if (!IsEnabled()) m_nTextHostState |= UISTATE_DISABLED;
		else m_nTextHostState &= ~UISTATE_DISABLED;
	}

	/************************************************************************/
	/* 显示编辑框文本内容                                                                     */
	/************************************************************************/
	void CEditTextHost::DrawText(HDC hDC,const RECT& rc,const CDuiString& sText)
	{
		RECT rcTextRange = {0};
		CRenderEngine::DrawText(hDC,GetManager(),rcTextRange,sText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);
		int nWidth = rcTextRange.right - rcTextRange.left;
		int nHeight = rcTextRange.bottom - rcTextRange.top;

		if( m_dwTextColor == 0 ) m_dwTextColor = GetManager()->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = GetManager()->GetDefaultDisabledColor();

		DWORD clColor = IsEnabled() ? ( IsWaterMode() ? m_dwWaterColor : m_dwTextColor) : m_dwDisabledTextColor;

		////在内存兼容设备上绘制文字
		HDC hMemDC=::CreateCompatibleDC(hDC);
		HBITMAP hBitmap = CreateCompatibleBitmap(hDC,nWidth,nHeight);
		SelectObject(hMemDC,hBitmap);

		BitBlt(hMemDC,rcTextRange.left+m_nCaretOffset,rcTextRange.top,rc.right-rc.left,rc.bottom-rc.top,hDC,rc.left,rc.top,SRCCOPY);
		CRenderEngine::DrawColor(hMemDC, rcTextRange, m_pOwner->GetBkColor());

		CRenderEngine::DrawText(hMemDC, GetManager(), rcTextRange, sText, clColor, m_iFont, DT_SINGLELINE | m_uTextStyle);

		//展示编辑框内容
		int nDstWidth = rc.right-rc.left;
		int nDstHeight = rc.bottom-rc.top;
		BitBlt(hDC,rc.left,rc.top,nDstWidth,nDstHeight,hMemDC,rcTextRange.left+m_nCaretOffset,rcTextRange.top,SRCCOPY);

		DeleteDC(hMemDC);
		DeleteObject(hBitmap);
	}

	/************************************************************************/
	/* 计算选择文字开始位置			                                                                   */
	/************************************************************************/
	void CEditTextHost::CalcStartSelRect(HDC hDC,const CDuiString& sText,int nPos,RECT& rcRange)
	{
		if (nPos == 0 ||sText.IsEmpty())
		{
			CDuiString strText(_T("EditUI"));
			CalcTextRect(hDC,strText,rcRange,0,1);
			rcRange.right = rcRange.left;
		}
		else
		{
			CalcTextRect(hDC,sText,rcRange,0,nPos);
			rcRange.left = rcRange.right;
		}
	}	

	/************************************************************************/
	/* 绘制选择区域文字                                                                     */
	/************************************************************************/
	void CEditTextHost::DrawSelectionText(HDC hDC,const RECT& rc,const CDuiString& sText)
	{
		//绘制选择区域文本
		int nFirstToRender = MIN(m_nSelStart,m_nCaretPos);
		int nNumChatToRender = MAX( m_nSelStart, m_nCaretPos ) - nFirstToRender;
		CDuiString strSelectionText = sText.Mid(nFirstToRender,nNumChatToRender);

		RECT rcSelection = {0};
		CRenderEngine::DrawText(hDC,GetManager(),rcSelection,strSelectionText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);

		HDC hSectionDC = CreateCompatibleDC(hDC);
		HBITMAP hSectionBitmap = CreateCompatibleBitmap(hDC,rcSelection.right - rcSelection.left,rcSelection.bottom - rcSelection.top);

		SelectObject(hSectionDC,hSectionBitmap);
		CRenderEngine::DrawColor(hSectionDC, rcSelection, m_dwSelectbkColor);
		CRenderEngine::DrawText(hSectionDC, GetManager(), rcSelection, strSelectionText, m_dwSelectTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);

		int nStartPos = 0;
		RECT rcStartSel = {0};
		CalcStartSelRect(hDC,sText, m_nSelStart, rcStartSel);

		int nOffset = 0;
		int nLenMin = 0;
		int nMinPos = 0;
		if (m_nCaretPos>m_nSelStart){
			nLenMin = MIN(rcSelection.right - rcSelection.left, rc.right - rcStartSel.left);
			nLenMin = MIN(nLenMin + m_nCaretOffset, rc.right - rc.left);
			nMinPos = rcStartSel.left - m_nCaretOffset;
		}
		else{
			nLenMin = MIN(rcSelection.right - rcSelection.left, rcStartSel.right - rc.left);
			nLenMin = MIN(nLenMin,rc.right - m_rcCaret.left);
			nMinPos = m_rcCaret.left;
		}

		if (nMinPos > rc.left)
			nStartPos = nMinPos;
		else{
			nStartPos = rc.left;
			nOffset = rc.left - (rcStartSel.left - m_nCaretOffset);
		}

		if (nOffset <=0)
			nOffset = 0;

		BitBlt(hDC,nStartPos,rc.top,nLenMin,rc.bottom-rc.top,hSectionDC,rcSelection.left+nOffset,rcSelection.top,SRCCOPY);

		DeleteDC(hSectionDC);
		DeleteObject(hSectionBitmap);
	}

	void CEditTextHost::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
	}

	DWORD CEditTextHost::GetTextColor() const
	{
		return m_dwTextColor;
	}

	void CEditTextHost::SetDisabledTextColor(DWORD dwTextColor)
	{
		m_dwDisabledTextColor = dwTextColor;
	}

	DWORD CEditTextHost::GetDisabledTextColor() const
	{
		return m_dwDisabledTextColor;
	}

	RECT CEditTextHost::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CEditTextHost::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
	}

	UINT CEditTextHost::GetTextState() const
	{
		return m_nTextHostState;
	}

	BOOL CEditTextHost::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CEditTextHost::SetReadOnly(BOOL bReadOnly/* = TRUE*/)
	{
		m_bReadOnly = bReadOnly;
	}

	BOOL CEditTextHost::IsPasswordMode() const
	{
		return m_bPasswordMode;
	}

	void CEditTextHost::SetPasswordMode(BOOL bPassword/* = TRUE*/)
	{
		m_bPasswordMode = bPassword;
	}

	void CEditTextHost::SetPasswordChar(LPCTSTR lpszPasswordChar)
	{
		if (m_cPasswordChar == lpszPasswordChar[0])
			return;

		m_cPasswordChar = lpszPasswordChar[0];
	}

	BOOL CEditTextHost::IsDigitalMode() const
	{
		return m_bDigitalMode;
	}

	void CEditTextHost::SetDigitalMode(BOOL bDigital/* = TRUE*/)
	{
		m_bDigitalMode = bDigital;
	}

	UINT CEditTextHost::GetLimitLength() const
	{
		return m_nLimitLength;
	}

	void CEditTextHost::SetLimitLength(UINT nLimitLength)
	{
		if (m_nLimitLength == nLimitLength)
			return;
		m_nLimitLength = nLimitLength;
	}

	BOOL CEditTextHost::IsEnabled() const
	{
		return m_pOwner->IsEnabled();
	}

	BOOL CEditTextHost::IsFocused() const
	{
		return m_bFocused;
	}

	CPaintManagerUI* CEditTextHost::GetManager()
	{
		return m_pOwner->GetManager();
	}

	void CEditTextHost::Invalidate()
	{
		if (m_pOwner == NULL)
			return;

		m_pOwner->Invalidate();
	}

	void CEditTextHost::SetFont(int iFont)
	{
		m_iFont = iFont;
	}

	void CEditTextHost::SetTextStyle(int uTextStyle)
	{
		m_uTextStyle = uTextStyle;
	}

	void CEditTextHost::SetCaretColor(DWORD dwCaretColor)
	{
		if (m_dwCaretColor == dwCaretColor)
			return;

		m_dwCaretColor = dwCaretColor;
	}

	void CEditTextHost::SetCaretWidth(int nCaretWidth)
	{
		if (m_nCaretWidth == nCaretWidth)
			return;

		m_nCaretWidth = nCaretWidth;
	}

	BOOL CEditTextHost::IsWaterMode() const
	{
		return m_bWaterMode;
	}

	void CEditTextHost::SetWaterMode(BOOL bWaterMode /*= TRUE*/)
	{
		m_bWaterMode = bWaterMode;
		m_sText = bWaterMode && IsFocused() == FALSE ? m_strWaterText : m_strTextBak;
	}

	void CEditTextHost::SetWaterText(LPCTSTR lpszWaterText)
	{
		if (m_strWaterText == lpszWaterText)
			return;

		m_strWaterText = lpszWaterText;
	}

	void CEditTextHost::SetWaterTextColor(DWORD dwWaterColor)
	{
		if (m_dwWaterColor == dwWaterColor)
			return;
		m_dwWaterColor = dwWaterColor;
	}

	void CEditTextHost::SetSelectBkColor(DWORD dwSelectBkColor)
	{
		if (m_dwSelectbkColor == dwSelectBkColor)
			return;

		m_dwSelectbkColor = dwSelectBkColor;
	}

	void CEditTextHost::SetSelectTextColor(DWORD dwSelectTextColor)
	{
		if (m_dwSelectTextColor == dwSelectTextColor)
			return;

		m_dwSelectTextColor = dwSelectTextColor;
	}

	BOOL CEditTextHost::AdjustImeWindowPos()
	{
		BOOL bSuccess = FALSE;
		do 
		{
			CPaintManagerUI* pManager = GetManager();
			HWND hWnd = pManager->GetPaintWindow();
			HIMC hIMC = ImmGetContext(hWnd);
			if (hIMC == NULL)
				break;

			POINT point;
			::GetCaretPos(&point);

			COMPOSITIONFORM Composition;
			Composition.dwStyle = CFS_POINT;
			Composition.ptCurrentPos.x = point.x;
			Composition.ptCurrentPos.y = point.y;
			ImmSetCompositionWindow(hIMC, &Composition);

			ImmReleaseContext(hWnd, hIMC);
			
			bSuccess = TRUE;
		} while (FALSE);
		
		return bSuccess;
	}

	CDuiString CEditTextHost::GetEditText() const
	{
		CDuiString strTextHost = m_sText;
		if (IsWaterMode())
			return strTextHost;
		if( IsPasswordMode() ) {
			strTextHost.Empty();
			LPCTSTR p = m_sText.GetData();
			while( *p != _T('\0') ) {
				strTextHost += m_cPasswordChar;
				p = ::CharNext(p);
			}
		}

		return strTextHost;
	}

	CDuiString CEditTextHost::GetSelectionText() const
	{
		CDuiString strText = GetEditText();
		//绘制选择区域文本
		int nFirstToRender = MIN(m_nSelStart,m_nCaretPos);
		int nNumChatToRender = MAX( m_nSelStart, m_nCaretPos ) - nFirstToRender;
		CDuiString strSelectionText = strText.Mid(nFirstToRender, nNumChatToRender);

		return strSelectionText;
	}

	void CEditTextHost::RestartCaretBlinkTimer()
	{
		CPaintManagerUI* pManager = GetManager();
		pManager->KillTimer(m_pOwner, IME_BLINK_TIMER_ID);
		pManager->SetTimer(m_pOwner, IME_BLINK_TIMER_ID, GetCaretBlinkTime());
		m_bCaretOn = TRUE;
	}

	BOOL CEditTextHost::OnKeyEvent(TEventUI& event)
	{
		BOOL bValidKey = TRUE;
		int nCalcCaretPos = m_nCaretPos;
		CDuiString sText =	GetEditText();
		int nLen = sText.GetLength();
		switch (event.chKey)
		{
			case VK_HOME:
				nCalcCaretPos = 0;
				break;
			case VK_END:
				nCalcCaretPos = nLen;
				break;
			case VK_UP:
			case VK_LEFT:
				nCalcCaretPos -= 1;
				break;
			case VK_DOWN:
			case VK_RIGHT:
				nCalcCaretPos += 1;
				break;
			case VK_BACK:
			case VK_DELETE:
				{
					m_bKeyDown = TRUE;
					int nMin = MIN(m_nSelStart, m_nCaretPos);
					int nMax = MAX(m_nSelStart, m_nCaretPos);
					if (nMax == nMin)
					{
						if (event.chKey == VK_BACK) --nMin;
						else	++nMax;
					}

					//重新构造编辑框内容
					CDuiString strRight = m_sText.Right(m_sText.GetLength() - nMax);
					CDuiString strLeft = m_sText.Left(nMin);
					CDuiString strValue;
					strValue.Format(_T("%s%s"), (LPCTSTR)strLeft, (LPCTSTR)strRight);
					SetText(strValue);
					nCalcCaretPos = nMin;
				}
				break;
			case VK_RETURN:
				if (m_pOwner)
					GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN, event.wParam, event.lParam);
				return false;
			case 'C':
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL)
				{
					const int nMin = MIN(m_nSelStart, m_nCaretPos);
					const int nMax = MAX(m_nSelStart, m_nCaretPos);
					OnCopy(nMin,nMax-nMin);
				}
				break;
			case 'V':
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL)
				{
					const int nMin = MIN(m_nSelStart, m_nCaretPos);
					const int nMax = MAX(m_nSelStart, m_nCaretPos);
					nCalcCaretPos = OnPaste(nMin, nMax);
					nLen = m_sText.GetLength();
					m_nSelStart = nCalcCaretPos;
				}
				break;
			case 'X':
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL)
				{
					const int nMin = MIN(m_nSelStart, m_nCaretPos);
					const int nMax = MAX(m_nSelStart, m_nCaretPos);
						OnCut(nMin, nMax - nMin);
				}
				break;
			case 'A':
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL)
					SelectAll();
				break;
			default:
					bValidKey = false;
				break;
		}

		if (nLen != m_sText.GetLength())
			GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);

		CDuiString strTipInfo;
		if (nCalcCaretPos < 0)
			nCalcCaretPos = 0;
		else if (nCalcCaretPos > nLen)
			nCalcCaretPos = nLen;

		m_nCaretPos = nCalcCaretPos;
		return bValidKey;
	}

	BOOL CEditTextHost::CanPaste()
	{
		return IsReadOnly() == FALSE;
	}

	void CEditTextHost::SelectAll()
	{
		CalcCaretType = CALC_CARET_TYPE_SELECT_ALL;
	}

	void CEditTextHost::OnCopy(int nPos, int nLen)
	{
		BOOL bSuccess = FALSE;
		HANDLE hMem = NULL;
		do 
		{
			bSuccess = OpenClipboard(GetManager()->GetPaintWindow());
			if (bSuccess == FALSE)
				break;

			if (EmptyClipboard() == 0)
				break;

			hMem = GlobalAlloc(GMEM_ZEROINIT,(nLen+1)*sizeof(TCHAR));
			if (hMem == NULL)
				break;

			LPTSTR lpText = (LPTSTR)GlobalLock(hMem);
			CDuiString strSrcText = CDuiString((LPCTSTR)m_sText + nPos,nLen);
			_tcscpy_s(lpText, nLen + 1, (LPCTSTR)strSrcText);
			GlobalUnlock(hMem);

			if (SetClipboardData(CF_UNICODETEXT, hMem) == FALSE)
				break;
		} while (FALSE);

		if (bSuccess)
			CloseClipboard();
	}

	int CEditTextHost::OnPaste(int nMin, int nMax)
	{
		int nLen = 0;
		if (CanPaste() == false)
			return nLen;

		CPaintManagerUI* pManager = GetManager();
		BOOL bSuccess = FALSE;
		HANDLE hMem = NULL;
		do 
		{
			bSuccess = OpenClipboard(pManager->GetPaintWindow());
			if (bSuccess == FALSE)
				break;

			HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
			if (hMem == NULL)
				break;

			CDuiString strLeft = m_sText.Left(nMin);
			CDuiString strRight = m_sText.Right(m_sText.GetLength() - nMax);
			CDuiString strMid = (LPCTSTR)GlobalLock(hMem);
			nLen = strMid.GetLength() + nMin;
			CDuiString strPaste;
			strPaste.Format(_T("%s%s%s"), (LPCTSTR)strLeft, (LPCTSTR)strMid, (LPCTSTR)strRight);
			//如果粘贴后的文字超过编辑框数量限制则失败
			if ((UINT)strPaste.GetLength() <= GetLimitLength())
				SetText(strPaste);
			else
				nLen = 0;
			GlobalUnlock(hMem);

			pManager->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
		} while (FALSE);

		if (bSuccess)
			CloseClipboard();

		return nLen;
	}

	void CEditTextHost::OnCut(int nPos, int nLen)
	{
		BOOL bSuccess = FALSE;
		HANDLE hMem = NULL;
		do 
		{
			bSuccess = OpenClipboard(GetManager()->GetPaintWindow());
			if (bSuccess == FALSE)
				break;

			if (EmptyClipboard() == 0)
				break;

			hMem = GlobalAlloc(GMEM_ZEROINIT,(nLen+1)*sizeof(TCHAR));
			if (hMem == NULL)
				break;

			CDuiString strLeft = m_sText.Left(nPos);
			CDuiString strRight = m_sText.Right(m_sText.GetLength() - nPos - nLen);
			CDuiString strCutText;
			strCutText.Format(_T("%s%s"), (LPCTSTR)strLeft, (LPCTSTR)strRight);
			SetText(strCutText);

			GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);

			LPTSTR lpText = (LPTSTR)GlobalLock(hMem);
			CDuiString strSrcText = CDuiString((LPCTSTR)m_sText + nPos, nLen);
			_tcscpy_s(lpText, nLen + 1, (LPCTSTR)strSrcText);
			GlobalUnlock(hMem);

			if (SetClipboardData(CF_UNICODETEXT, hMem) == FALSE)
				break;

		} while (FALSE);

		if (bSuccess)
			CloseClipboard();
	}
}