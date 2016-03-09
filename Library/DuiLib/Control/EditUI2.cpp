#include "stdafx.h"
#include "EditUI2.h"

namespace DuiLib
{
#define    IME_BLINK_TIMER_ID			1000

	CEditUI2::CEditUI2()
		:m_uMaxChar(-1)
		,m_bReadOnly(false)
		,m_bPasswordMode(false)
		,m_bDigitalMode(false)
		,m_bDeleteKeyDown(false)
		,m_bCaretOn(true)
		,m_bWaterMode(true)
		,m_cPasswordChar(_T('*'))
		,m_nEditState(0)
		,m_nSelStart(0)
		,m_nCaretPos(0)
		,m_nCaretOffset(0)
		,m_nCaretWidth(1)
		,m_nLimitText(-1)
		,m_dwSelectTextColor(0xFFFFFFFF)
		,m_dwSelectbkColor(0xFF3399FF)
		,m_dwWaterColor(0xFFBAC0C5)
		,CalcCaretType(CAC_CARET_TYPE_NONE)
	{
		SetRect(&m_rcTextPadding,5,4,5,2);
		ZeroMemory(&m_rcCaret,sizeof(RECT));
		ZeroMemory(&m_szCaretPt,sizeof(POINT));
		m_uTextStyle = DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX;
	}

	CEditUI2::~CEditUI2()
	{

	}

	LPCTSTR CEditUI2::GetClass() const
	{
		return _T("EditUI2");
	}

	LPVOID CEditUI2::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName,DUI_CTR_EDIT_EX) == 0)
			return static_cast<CEditUI2*>(this);
		else
			return CLabelUI::GetInterface(pstrName);
	}

	UINT CEditUI2::GetControlFlags() const
	{
		if( !IsEnabled() ) return CLabelUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP | UIFLAG_IME_COMPOSITION;
	}

	void CEditUI2::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName,_T("readonly")) == 0)		SetReadOnly(_tcsicmp(pstrValue,_T("true")) == 0);
		else if (_tcsicmp(pstrName,_T("password")) == 0)	SetPasswordMode(_tcsicmp(pstrValue,_T("true")) == 0);
		else if (_tcsicmp(pstrName,_T("passwordchar")) == 0)	SetPasswordChar(pstrValue);
		else if (_tcsicmp(pstrName,_T("digital")) == 0)	SetDigitalMode(_tcsicmp(pstrValue,_T("true")) == 0);
		else if (_tcsicmp(pstrName,_T("limited")) == 0) SetLimitText(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName,_T("normalimage")) == 0)	SetNormalImage(pstrValue);
		else if (_tcsicmp(pstrName,_T("hotimage")) == 0)	SetHotImage(pstrValue);
		else if (_tcsicmp(pstrName,_T("focusedimage")) == 0)  SetFocusedImage(pstrValue);
		else if (_tcsicmp(pstrName,_T("disabledimage")) == 0) SetDisabledImage(pstrValue);
		else if (_tcsicmp(pstrName,_T("caretwidth")) == 0) SetCaretWidth(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName,_T("watertext")) == 0) SetWaterText(pstrValue);
		else if (_tcsicmp(pstrName,_T("text")) == 0 ) SetText(pstrValue);
		else if (_tcsicmp(pstrName,_T("watercolor")) == 0) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetWaterTextColor(clrColor);
		}
		else if (_tcsicmp(pstrName,_T("selecttextcolor")) == 0){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectTextColor(clrColor);
		}
		else if (_tcsicmp(pstrName,_T("selectbkcolor")) == 0 ){
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectBkColor(clrColor);
		}
		else
			CLabelUI::SetAttribute(pstrName,pstrValue);
	}

	void CEditUI2::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETCURSOR)
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}

		if (event.Type == UIEVENT_TIMER)
		{
			m_bCaretOn = !m_bCaretOn;
			InvalidateRect(m_pManager->GetPaintWindow(),&m_rcCaret,FALSE);
			return;
		}
		
		if (event.Type == UIEVENT_SETFOCUS){
			if (IsEnabled()){
				m_bFocused = true;
				SetWaterMode(false);
				m_pManager->SetTimer(this,IME_BLINK_TIMER_ID,GetCaretBlinkTime());
				CreateCaret(m_pManager->GetPaintWindow(),NULL,2,2);
				Invalidate();
			}
			return;
		}else if (event.Type == UIEVENT_KILLFOCUS){
			if (IsEnabled()){
				m_nSelStart = m_nCaretPos;
				m_nCaretOffset = 0;
				m_bCaretOn = false;
				m_bFocused = false;

				if (m_sText.IsEmpty())
					SetWaterMode(true);

				m_pManager->KillTimer(this,IME_BLINK_TIMER_ID);
				DestroyCaret();
				Invalidate();
			}
			return;
		}

		if( event.Type == UIEVENT_MOUSEENTER ){
			if( IsEnabled() ) {
				m_nEditState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}else if( event.Type == UIEVENT_MOUSELEAVE ){
			if( IsEnabled() ) {
				m_nEditState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_KEYDOWN){
			if (IsEnabled()){
				if (IsReadOnly())
					return;

				if (OnKeyEvent(event) && (event.chKey < 'A' || event.chKey > 'Z'))
				{
					if ((event.wKeyState & MK_CONTROL) != MK_CONTROL &&
						(event.wKeyState & MK_SHIFT) != MK_SHIFT)
						m_nSelStart = m_nCaretPos;
					RestartCaretBlinkTimer();
				}
				Invalidate();
			}
			return;
		}

		if (event.Type == UIEVENT_CHAR){
			
			if (event.chKey == VK_BACK || event.chKey == VK_RETURN || event.chKey == VK_ESCAPE)
				return;
			if (event.wKeyState && (event.wKeyState & MK_SHIFT) != MK_SHIFT)
				return;

			if (IsEnabled()){
				if (IsReadOnly())
					return;

				if (IsDigitalMode() == true && (event.chKey < 256 && isdigit(event.chKey)) == FALSE)
					return;

				int nMin = __min(m_nSelStart,m_nCaretPos);
				int nMax = __max(m_nSelStart,m_nCaretPos);
				CDuiString strLeft = m_sText.Left(nMin);
				CDuiString strRight = m_sText.Right(m_sText.GetLength()-nMax);
				CDuiString strValue;
				strValue.Format(_T("%s%c%s"),(LPCTSTR)strLeft,event.chKey,(LPCTSTR)strRight);
				if ((UINT)strValue.GetLength()<=GetLimitText())
				{
					SetText(strValue);
					m_nSelStart = m_nCaretPos = nMin+1;
					//SaveEditText(m_sText);
					RestartCaretBlinkTimer();
					m_pManager->SendNotify(this,DUI_MSGTYPE_TEXTCHANGED);
					Invalidate();
				}
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE){
			if ((m_nEditState & UISTATE_PUSHED) != UISTATE_PUSHED)
				return;
			//确定选择区域
			CalcCaretType = CALC_CARET_TYPE_SELECT_MODE;
			m_szCaretPt.x = event.ptMouse.x;
			m_szCaretPt.y = event.ptMouse.y;
			Invalidate();
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU ){
			if (IsContextMenuUsed())
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			return;
		}
		//调整输入法窗口位置
		if (event.Type == UIEVENT_IME_STARTCOMPOSITION)
			AdjustImeWindowPos();

		/*if (event.Type == UIEVENT_IME_COMPOSITION)
		{
			if ((event.lParam & GCS_RESULTSTR) == 0)
				return;
			HIMC hIMC;
			DWORD dwSize;
			HGLOBAL hGlobal;
			LPTSTR lpEditText;
			hIMC = ImmGetContext(m_pManager->GetPaintWindow());
			if (hIMC == NULL)
				return;
			dwSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
			dwSize += sizeof(TCHAR);
			hGlobal = GlobalAlloc(GHND,dwSize);
			lpEditText = (LPTSTR)GlobalLock(hGlobal);
			ImmGetCompositionString(hIMC, GCS_RESULTSTR, lpEditText, dwSize);
			ImmReleaseContext(m_pManager->GetPaintWindow(), hIMC);
			int nMin = __min(m_nSelStart,m_nCaretPos);
			int nMax = __max(m_nSelStart,m_nCaretPos);
			LPCTSTR lpszLeft = m_sText.Left(nMin);
			LPCTSTR lpszRight = m_sText.Right(m_sText.GetLength()-nMax);
			m_sText.Format(_T("%s%s%s"),lpszLeft,lpEditText,lpszRight);
			m_nCaretPos += _tcslen(lpEditText);
			m_nSelStart = m_nCaretPos;
			GlobalUnlock(hGlobal);
			GlobalFree(hGlobal);
			Invalidate();
			return;
		}*/

		if (event.Type == UIEVENT_DBLCLICK){
			if (IsEnabled()){
				m_nSelStart = 0;
				m_nCaretPos = m_sText.GetLength();
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_BUTTONDOWN){
			if (IsEnabled()){
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL || (event.wKeyState & MK_SHIFT) == MK_SHIFT)
					CalcCaretType = CALC_CARET_TYPE_SELECT_MODE;
				else
					CalcCaretType = CALC_CARET_TYPE_POS;
				m_nEditState |= UISTATE_PUSHED;
				m_szCaretPt.x = event.ptMouse.x;
				m_szCaretPt.y = event.ptMouse.y;
				RestartCaretBlinkTimer();
				Invalidate();
			}
			return;
		}

		if (event.Type == UIEVENT_BUTTONUP){
			if (IsEnabled()){
				m_nEditState &= ~UISTATE_PUSHED;
				CalcCaretType = CAC_CARET_TYPE_NONE;
				Invalidate();
			}			
			return;
		}
		CLabelUI::DoEvent(event);
	}

	void CEditUI2::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_nEditState |= UISTATE_FOCUSED;
		else m_nEditState &= ~UISTATE_FOCUSED;
		if( !IsEnabled() ) m_nEditState |= UISTATE_DISABLED;
		else m_nEditState &= ~UISTATE_DISABLED;

		if (!m_sBkImage.IsEmpty()){
			if (!DrawImage(hDC,(LPCTSTR)m_sBkImage)) m_sBkImage.Empty();
			else CRenderEngine::DrawColor(hDC,m_rcItem,m_dwBackColor);
		}

		if( (m_nEditState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
				else return;
			}
		}
		else if( (m_nEditState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else return;
			}
		}
		else if( (m_nEditState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
				else return;
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
			else return;
		}
	}

	void CEditUI2::PaintBorder(HDC hDC)
	{
		PaintCaret(hDC);
	}

	void CEditUI2::PaintCaret(HDC hDC)
	{
		if (m_nEditState & UISTATE_FOCUSED && m_bCaretOn)
		{
			CRenderEngine::DrawColor(hDC,m_rcCaret,m_dwCaretColor);
			SetCaretPos(m_rcCaret.left,m_rcCaret.top+(m_rcCaret.bottom-m_rcCaret.top)/2);

			AdjustImeWindowPos();
		}
	}

	void CEditUI2::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		CDuiString sText;
		GetEditText(sText);

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		if ((CalcCaretType & CALC_CARET_TYPE_POS) == CALC_CARET_TYPE_POS)
		{
			m_nCaretPos = GetCaretPos(hDC,sText);
			m_nSelStart = m_nCaretPos;
			CalcCaretType = CAC_CARET_TYPE_NONE;
		}
		else if ((CalcCaretType & CALC_CARET_TYPE_SELECT_MODE) == CALC_CARET_TYPE_SELECT_MODE)
		{
			m_nCaretPos = GetCaretPos(hDC,sText);
			CalcCaretType = CAC_CARET_TYPE_NONE;
		}

		if (m_nEditState & UISTATE_FOCUSED)
			CalcCaretRect(hDC,sText);

		if( sText.IsEmpty() ) return;

		DrawText(hDC,rc,sText);

		if( m_nCaretPos != m_nSelStart )
			DrawSelectionText(hDC,rc,sText);
	}

	void CEditUI2::DrawText(HDC hDC,const RECT& rc,const CDuiString& sText)
	{
		RECT rcTextRange = {0};
		CRenderEngine::DrawText(hDC,m_pManager,rcTextRange,sText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);
		int nWidth = rcTextRange.right - rcTextRange.left;
		int nHeight = rcTextRange.bottom - rcTextRange.top;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		DWORD clColor = IsEnabled() ? ( IsWaterMode() ? m_dwWaterColor : m_dwTextColor) : m_dwDisabledTextColor;

		////在内存兼容设备上绘制文字
		HDC hMemDC=::CreateCompatibleDC(hDC);
		HBITMAP hBitmap = CreateCompatibleBitmap(hDC,nWidth,nHeight);
		SelectObject(hMemDC,hBitmap);
		
		BitBlt(hMemDC,rcTextRange.left+m_nCaretOffset,rcTextRange.top,rc.right-rc.left,rc.bottom-rc.top,hDC,rc.left,rc.top,SRCCOPY);
		CRenderEngine::DrawColor(hMemDC,rcTextRange,m_dwBackColor);

		CRenderEngine::DrawText(hMemDC, m_pManager, rcTextRange, sText, clColor, m_iFont, DT_SINGLELINE | m_uTextStyle);

		//展示编辑框内容
		int nDstWidth = rc.right-rc.left;
		int nDstHeight = rc.bottom-rc.top;
		BitBlt(hDC,rc.left,rc.top,nDstWidth,nDstHeight,hMemDC,rcTextRange.left+m_nCaretOffset,rcTextRange.top,SRCCOPY);

		DeleteDC(hMemDC);
		DeleteObject(hBitmap);
	}

	void CEditUI2::DrawSelectionText(HDC hDC,const RECT& rc,const CDuiString& sText)
	{
		//绘制选择区域文本
		int nFirstToRender = __min(m_nSelStart,m_nCaretPos);
		int nNumChatToRender = __max( m_nSelStart, m_nCaretPos ) - nFirstToRender;
		CDuiString strSelectionText = sText.Mid(nFirstToRender,nNumChatToRender);

		RECT rcSelection = {0};
		CRenderEngine::DrawText(hDC,m_pManager,rcSelection,strSelectionText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);

		HDC hSectionDC = CreateCompatibleDC(hDC);
		HBITMAP hSectionBitmap = CreateCompatibleBitmap(hDC,rcSelection.right - rcSelection.left,rcSelection.bottom - rcSelection.top);

		SelectObject(hSectionDC,hSectionBitmap);
		CRenderEngine::DrawColor(hSectionDC,rcSelection,m_dwSelectbkColor);
		CRenderEngine::DrawText(hSectionDC, m_pManager, rcSelection, strSelectionText, m_dwSelectTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);

		int nStartPos = 0;
		RECT rcStartSel = {0};
		CalcStartSelRect(hDC,sText,m_nSelStart,rcStartSel);

		int nOffset = 0;
		int nLenMin = 0;
		int nMinPos = 0;
		if (m_nCaretPos>m_nSelStart){
			nLenMin = __min(rcSelection.right - rcSelection.left,rc.right-rcStartSel.left);
			nLenMin = __min(nLenMin+m_nCaretOffset,rc.right-rc.left);
			nMinPos = rcStartSel.left - m_nCaretOffset;
		}
		else{
			nLenMin = __min(rcSelection.right - rcSelection.left,rcStartSel.right-rc.left);
			nLenMin = __min(nLenMin,rc.right-m_rcCaret.left);
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

	bool CEditUI2::IsReadOnly()
	{
		return m_bReadOnly;
	}

	void CEditUI2::SetReadOnly(bool breadonly)
	{
		m_bReadOnly = breadonly;
	}

	void CEditUI2::SetPasswordMode(bool bpassword)
	{
		m_bPasswordMode = bpassword;
	}

	void CEditUI2::SetPasswordChar(LPCTSTR lpszPasswordChar)
	{
		if (m_cPasswordChar == lpszPasswordChar[0])
			return;

		m_cPasswordChar = lpszPasswordChar[0];
		Invalidate();
	}

	bool CEditUI2::IsDigitalMode()
	{
		return m_bDigitalMode;
	}

	void CEditUI2::SetDigitalMode(bool bdigital)
	{
		m_bDigitalMode = bdigital;
	}

	UINT CEditUI2::GetLimitText()
	{
		return m_nLimitText;
	}

	void CEditUI2::SetLimitText(UINT nLimitText)
	{
		if (m_nLimitText == nLimitText)
			return;
		m_nLimitText = nLimitText;
	}

	void CEditUI2::SetDisabledImage(LPCTSTR lpszDisabledImage)
	{
		if (m_sDisabledImage == lpszDisabledImage)
			return;
		m_sDisabledImage = lpszDisabledImage;
		Invalidate();
	}

	void CEditUI2::SetSelectBkColor(DWORD dwSelectBkColor)
	{
		if (m_dwSelectbkColor == dwSelectBkColor)
			return;

		m_dwSelectbkColor = dwSelectBkColor;
		Invalidate();
	}

	void CEditUI2::SetSelectTextColor(DWORD dwSelectTextColor)
	{
		if (m_dwSelectTextColor == dwSelectTextColor)
			return;

		m_dwSelectTextColor = dwSelectTextColor;
		Invalidate();
	}

	void CEditUI2::SetNormalImage(LPCTSTR lpszNormalImage)
	{
		if (m_sNormalImage == lpszNormalImage)
			return;
		m_sNormalImage = lpszNormalImage;
		Invalidate();
	}

	void CEditUI2::SetHotImage(LPCTSTR lpszHotImage)
	{
		if (m_sHotImage == lpszHotImage)
			return;
		m_sHotImage = lpszHotImage;
		Invalidate();
	}

	void CEditUI2::SetFocusedImage(LPCTSTR lpszFocusedImage)
	{
		if (m_sFocusedImage == lpszFocusedImage)
			return;
		m_sFocusedImage = lpszFocusedImage;
		Invalidate();
	}

	void CEditUI2::SetCaretWidth(int nCaretWidth)
	{
		if (m_nCaretWidth == nCaretWidth)
			return;
		m_nCaretWidth = nCaretWidth;
		Invalidate();
	}

	void CEditUI2::SetWaterTextColor(DWORD dwWaterColor)
	{
		if (m_dwWaterColor == dwWaterColor)
			return;
		m_dwWaterColor = dwWaterColor;
		Invalidate();
	}

	void CEditUI2::SetWaterText(LPCTSTR lpszWaterText)
	{
		if (m_sWaterText == lpszWaterText)
			return;
		m_sWaterText = lpszWaterText;
		SetWaterMode(true);
		Invalidate();
	}

	bool CEditUI2::IsWaterMode()
	{
		return m_bWaterMode;
	}

	void CEditUI2::SetWaterMode(bool bWaterMode)
	{
		m_bWaterMode = bWaterMode;

		m_sText = bWaterMode && IsFocused() == false ? m_sWaterText : m_sTextBak;
	}

	void CEditUI2::SetText(LPCTSTR pstrText)
	{
		if (m_sText == pstrText)
			return;

		m_sTextBak = pstrText;
		SetWaterMode(m_sTextBak.IsEmpty() == TRUE);
		Invalidate();
	}

	void CEditUI2::SaveEditText(LPCTSTR lpszEditText)
	{
		if (IsWaterMode() == false && m_sTextBak == lpszEditText)
			return;
		m_sTextBak = lpszEditText;
	}

	void CEditUI2::CalcTextRect(HDC hDC,const CDuiString& strText,RECT& rcSelection,int nStart,int nLen)
	{
		ZeroMemory(&rcSelection,sizeof(RECT));
		CDuiString strSelectionText;
		if (nStart == 0)
			strSelectionText = strText.Left(nLen);
		else 
			strSelectionText = strText.Mid(nStart,nLen);

		CRenderEngine::DrawText(hDC,m_pManager,rcSelection,strSelectionText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);

		UINT nWidth = rcSelection.right - rcSelection.left;
		UINT nHeight = rcSelection.bottom - rcSelection.top;

		if (nStart != 0)
		{
			RECT rcSelectionOffset = {0};
			strSelectionText = strText.Left(nStart);
			CRenderEngine::DrawText(hDC,m_pManager,rcSelectionOffset,strSelectionText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);
			
			rcSelection.left += rcSelectionOffset.right;
		}

		rcSelection.left += m_rcItem.left + m_rcTextPadding.left;
		rcSelection.top += m_rcItem.top + m_rcTextPadding.top;
		rcSelection.right = rcSelection.left +nWidth;
		rcSelection.bottom = rcSelection.top + nHeight ;
	}

	int CEditUI2::GetCaretPos(HDC hDC,const CDuiString& sText)
	{
		int nCaretPos = 0;
		do 
		{
			RECT rcCaretPos = {0};
			for (int n=0;n<sText.GetLength();++n)
			{
				CalcTextRect(hDC,sText,rcCaretPos,n,1);
				int nWidth = rcCaretPos.right-rcCaretPos.left;
				rcCaretPos.left -= m_nCaretOffset;
				rcCaretPos.right = rcCaretPos.left + nWidth;
				if (PtInRect(&rcCaretPos,m_szCaretPt))
				{
					int nLeft = m_szCaretPt.x - rcCaretPos.left;
					int nRight = rcCaretPos.right - m_szCaretPt.x;
					int nMin = __min(nLeft,nRight);
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
		} while (FALSE);

		return nCaretPos;
	}

	bool CEditUI2::OnKeyEvent(TEventUI& event)
	{
		bool bValidKey = true;
		int nCalcCaretPos = m_nCaretPos;
		CDuiString sText;
		GetEditText(sText);
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
					m_bDeleteKeyDown = true;
					int nMin = __min(m_nSelStart,m_nCaretPos);
					int nMax = __max(m_nSelStart,m_nCaretPos);
					if (nMax == nMin)
					{
						if (event.chKey == VK_BACK) --nMin;
						else	++nMax;
					}

					CDuiString strRight = m_sText.Right(m_sText.GetLength()-nMax);
					CDuiString strLeft = m_sText.Left(nMin);
					CDuiString strValue;
					strValue.Format(_T("%s%s"),(LPCTSTR)strLeft,(LPCTSTR)strRight);
					SetText(strValue);
					nCalcCaretPos = nMin;
				}
				break;
			case VK_RETURN:
				if (m_pManager)
					m_pManager->SendNotify(this,DUI_MSGTYPE_RETURN,event.wParam,event.lParam);
				return false;
			case 'C':
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL)
				{
					const int nMin = __min(m_nSelStart,m_nCaretPos);
					const int nMax = __max(m_nSelStart,m_nCaretPos);
					OnCopy(nMin,nMax-nMin);
				}
				break;
			case 'V':
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL)
				{
					const int nMin = __min(m_nSelStart,m_nCaretPos);
					const int nMax = __max(m_nSelStart,m_nCaretPos);
					nCalcCaretPos = OnPaste(nMin,nMax);
					nLen = m_sText.GetLength();
					m_nSelStart = nCalcCaretPos;
				}
				break;
			case 'X':
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL)
				{
					const int nMin = __min(m_nSelStart,m_nCaretPos);
					const int nMax = __max(m_nSelStart,m_nCaretPos);
					OnCut(nMin,nMax-nMin);
				}
				break;
			case 'A':
				if ((event.wKeyState & MK_CONTROL) == MK_CONTROL)
					OnCheckAll();
				break;
			default:
					bValidKey = false;
				break;
		}

		if (nLen != m_sText.GetLength())
			m_pManager->SendNotify(this,DUI_MSGTYPE_TEXTCHANGED);

		CDuiString strTipInfo;
		if (nCalcCaretPos < 0)
			nCalcCaretPos = 0;
		else if (nCalcCaretPos > nLen)
			nCalcCaretPos = nLen;

		m_nCaretPos = nCalcCaretPos;
		return bValidKey;
	}

	bool CEditUI2::GetEditText(CDuiString& strEditText)
	{
		strEditText = m_sText;
		if (m_bWaterMode)
			return true;
		if( m_bPasswordMode ) {
			strEditText.Empty();
			LPCTSTR p = m_sText.GetData();
			while( *p != _T('\0') ) {
				strEditText += m_cPasswordChar;
				p = ::CharNext(p);
			}
		}

		return true;
	}

	bool CEditUI2::AdjustImeWindowPos()
	{
		HIMC hIMC = ImmGetContext(m_pManager->GetPaintWindow());
		if (hIMC == NULL)
			return false;

		POINT point;
		::GetCaretPos(&point);

		COMPOSITIONFORM Composition;
		Composition.dwStyle = CFS_POINT;
		Composition.ptCurrentPos.x = point.x;
		Composition.ptCurrentPos.y = point.y;
		ImmSetCompositionWindow(hIMC, &Composition);

		ImmReleaseContext(m_pManager->GetPaintWindow(),hIMC);
		return true;
	}

	void CEditUI2::RestartCaretBlinkTimer()
	{
		m_pManager->KillTimer(this,IME_BLINK_TIMER_ID);
		m_pManager->SetTimer(this,IME_BLINK_TIMER_ID,GetCaretBlinkTime());
		/*m_dfLastBlink = m_dfBlink = 0;*/
		m_bCaretOn = true;
	}

	void CEditUI2::SetPos(RECT rc)
	{
		CLabelUI::SetPos(rc);
	}

	void CEditUI2::FixedCaretPos(int nFixed)
	{
		m_rcCaret.left -= nFixed;
		m_rcCaret.right = m_rcCaret.left + m_nCaretWidth;
	}

	void CEditUI2::CalcStartSelRect(HDC hDC,const CDuiString& sText,int nPos,RECT& rcRange)
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

	void CEditUI2::CalcCaretRect(HDC hDC,const CDuiString& sText)
	{
		static DWORD dwOffset = 0;

		if (m_nCaretPos == 0 ||sText.IsEmpty())
		{
			CDuiString strText = _T("EditUI");
			CalcTextRect(hDC,strText,m_rcCaret,0,1);
			m_rcCaret.right = m_rcCaret.left + m_nCaretWidth;
		}
		else
		{
			CalcTextRect(hDC,sText,m_rcCaret,0,m_nCaretPos);
			m_rcCaret.left = m_rcCaret.right;
			m_rcCaret.right += m_nCaretWidth;
		}

		//计算输入光标相对于编辑框的偏移量
		if (m_bDeleteKeyDown)
		{
			RECT rcTextRange = {0};
		 	CDuiString strDrawText = sText.Left(m_nCaretPos);
			CRenderEngine::DrawText(hDC,m_pManager,rcTextRange,strDrawText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);

			m_nCaretOffset = rcTextRange.right - rcTextRange.left - dwOffset;
		
			if (m_nCaretOffset > 0 )
			{
				CRenderEngine::DrawText(hDC,m_pManager,rcTextRange,sText,0,m_iFont,DT_CALCRECT|DT_NOPREFIX);
				LONG lTextRight = rcTextRange.right + m_rcItem.left + m_rcTextPadding.left - m_nCaretOffset;
				if (lTextRight < m_rcItem.right - m_rcTextPadding.right)
					m_nCaretOffset -= m_rcItem.right - m_rcTextPadding.right - lTextRight - m_nCaretWidth;
			}

			if (m_nCaretOffset <= 0)
				m_nCaretOffset = 0;
			FixedCaretPos(m_nCaretOffset);

			m_bDeleteKeyDown = false;
		}
		else if (m_rcCaret.left <= m_rcItem.left + m_rcTextPadding.left + m_nCaretOffset)
		{
			m_nCaretOffset = m_rcCaret.left - m_rcTextPadding.left - m_rcItem.left;
			FixedCaretPos(m_nCaretOffset);
		}
		else if (m_rcCaret.right > m_rcItem.right - m_rcTextPadding.right && (m_rcItem.right  - m_rcTextPadding.right + m_nCaretOffset < m_rcCaret.right))
		{
			m_nCaretOffset = m_rcCaret.right - m_rcItem.right + m_rcTextPadding.right;
			FixedCaretPos(m_nCaretOffset);
		}
		else if (m_nCaretOffset)
		{
			FixedCaretPos(m_nCaretOffset);
		}

		dwOffset = m_rcCaret.left - m_rcItem.left - m_rcTextPadding.left;
	}

	bool CEditUI2::CanPaste()
	{
		return IsReadOnly() == false;
	}

	void CEditUI2::OnCheckAll()
	{
		
	}

	void CEditUI2::OnCopy(int nPos,int nLen)
	{
		BOOL bSuccess = FALSE;
		HANDLE hMem = NULL;
		do 
		{
			bSuccess = OpenClipboard(m_pManager->GetPaintWindow());
			if (bSuccess == FALSE)
				break;

			if (EmptyClipboard() == 0)
				break;

			hMem = GlobalAlloc(GMEM_ZEROINIT,(nLen+1)*sizeof(TCHAR));
			if (hMem == NULL)
				break;

			LPTSTR lpText = (LPTSTR)GlobalLock(hMem);
			CDuiString strSrcText = CDuiString((LPCTSTR)m_sText+nPos,nLen);
			_tcscpy_s(lpText,nLen+1,(LPCTSTR)strSrcText);
			GlobalUnlock(hMem);

			if (SetClipboardData(CF_UNICODETEXT,hMem) == FALSE)
				break;
		} while (FALSE);
		
		if (bSuccess)
			CloseClipboard();
	}

	int CEditUI2::OnPaste(int nMin,int nMax)
	{
		int nLen = 0;
		if (CanPaste() == false)
			return nLen;

		BOOL bSuccess = FALSE;
		HANDLE hMem = NULL;
		do 
		{
			bSuccess = OpenClipboard(m_pManager->GetPaintWindow());
			if (bSuccess == FALSE)
				break;

			HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
			if (hMem == NULL)
				break;

			CDuiString strLeft = m_sText.Left(nMin);
			CDuiString strRight = m_sText.Right(m_sText.GetLength()-nMax);
			CDuiString strMid = (LPCTSTR)GlobalLock(hMem);
			nLen = strMid.GetLength()+nMin;
			CDuiString strPaste;
			strPaste.Format(_T("%s%s%s"),(LPCTSTR)strLeft,(LPCTSTR)strMid,(LPCTSTR)strRight);
			//如果粘贴后的文字超过编辑框数量限制则失败
			if ((UINT)strPaste.GetLength() <= GetLimitText())
				SetText(strPaste);
			else
				nLen = 0;
			GlobalUnlock(hMem);

		} while (FALSE);

		if (bSuccess)
			CloseClipboard();

		return nLen;
	}

	void CEditUI2::OnCut(int nPos,int nLen)
	{
		BOOL bSuccess = FALSE;
		HANDLE hMem = NULL;
		do 
		{
			bSuccess = OpenClipboard(m_pManager->GetPaintWindow());
			if (bSuccess == FALSE)
				break;

			if (EmptyClipboard() == 0)
				break;

			hMem = GlobalAlloc(GMEM_ZEROINIT,(nLen+1)*sizeof(TCHAR));
			if (hMem == NULL)
				break;

			CDuiString strLeft = m_sText.Left(nPos);
			CDuiString strRight = m_sText.Right(m_sText.GetLength()-nPos-nLen);
			CDuiString strCutText;
			strCutText.Format(_T("%s%s"),(LPCTSTR)strLeft,(LPCTSTR)strRight);
			SetText(strCutText);

			LPTSTR lpText = (LPTSTR)GlobalLock(hMem);
			CDuiString strSrcText = CDuiString((LPCTSTR)m_sText+nPos,nLen);
			_tcscpy_s(lpText,nLen+1,(LPCTSTR)strSrcText);
			GlobalUnlock(hMem);

			if (SetClipboardData(CF_UNICODETEXT,hMem) == FALSE)
				break;

		} while (FALSE);

		if (bSuccess)
			CloseClipboard();
	}
}
