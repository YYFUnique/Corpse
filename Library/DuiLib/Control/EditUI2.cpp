#include "stdafx.h"
#include "EditUI2.h"

namespace DuiLib
{
	CEditUI2::CEditUI2()
		: m_pTextHost(NULL)
	{
		m_CursorType = IDC_IBEAM;
		m_pTextHost = new CEditTextHost;
	}

	CEditUI2::~CEditUI2()
	{
		if (m_pTextHost != NULL)
			delete m_pTextHost;
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

	void CEditUI2::DoInit()
	{
		m_pTextHost->SetOwner(this);

		m_pTextHost->SetTextStyle(m_uTextStyle);
		m_pTextHost->SetFont(GetFont());
		m_pTextHost->SetTextColor(m_dwTextColor);
		m_pTextHost->SetDisabledTextColor(m_dwDisabledTextColor);
		m_pTextHost->SetTextPadding(m_rcTextPadding);
		m_pTextHost->SetCaretColor(m_dwCaretColor);
	}

	CDuiString CEditUI2::GetText() const
	{
		return m_pTextHost->GetText();
	}

	void CEditUI2::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName,_T("readonly")) == 0)	 SetReadOnly(_tcsicmp(pstrValue,_T("true")) == 0);
		else if (_tcsicmp(pstrName,_T("password")) == 0)	SetPasswordMode(_tcsicmp(pstrValue,_T("true")) == 0);
		else if (_tcsicmp(pstrName,_T("passwordchar")) == 0)	SetPasswordChar(pstrValue);
		else if (_tcsicmp(pstrName,_T("digital")) == 0)	SetDigitalMode(_tcsicmp(pstrValue,_T("true")) == 0);
		else if (_tcsicmp(pstrName,_T("limited")) == 0) SetLimitLength(_ttoi(pstrValue));
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

		if (m_pTextHost->DoEvent(event) != FALSE)
			return;

		if (event.Type == UIEVENT_DBLCLICK) {
			if (IsEnabled()){
				GetManager()->SendNotify(this, DUI_MSGTYPE_DBCLICK, event.wParam, event.lParam);
				Invalidate();
			}
		} else if( event.Type == UIEVENT_CONTEXTMENU ){
			if (IsContextMenuUsed() && IsEnabled())
				GetManager()->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
		} else
			CLabelUI::DoEvent(event);
	}

	void CEditUI2::PaintStatusImage(HDC hDC)
	{
		m_pTextHost->PaintStatusImage(hDC);
		UINT nTextState = m_pTextHost->GetTextState();

		if (!m_sBkImage.IsEmpty()){
			if (!DrawImage(hDC,(LPCTSTR)m_sBkImage)) m_sBkImage.Empty();
			else CRenderEngine::DrawColor(hDC,m_rcItem,m_dwBackColor);
		}

		if( (nTextState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
				else return;
			}
		}
		else if( (nTextState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else return;
			}
		}
		else if( (nTextState & UISTATE_HOT) != 0 ) {
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
		m_pTextHost->PaintCaret(hDC);
	}

	void CEditUI2::PaintText(HDC hDC)
	{
		m_pTextHost->PaintText(hDC);
	}

	BOOL CEditUI2::IsReadOnly() const
	{
		return m_pTextHost->IsReadOnly();
	}

	void CEditUI2::SetReadOnly(BOOL bReadonly)
	{
		if (IsReadOnly() == bReadonly)
			return;

		m_pTextHost->SetReadOnly(bReadonly);
	}

	BOOL CEditUI2::IsPasswordMode() const
	{
		return m_pTextHost->IsPasswordMode();
	}

	void CEditUI2::SetPasswordMode(BOOL bPassword)
	{
		if (IsPasswordMode() == bPassword)
			return;

		m_pTextHost->SetPasswordMode(bPassword);
	}

	void CEditUI2::SetPasswordChar(LPCTSTR lpszPasswordChar)
	{
		m_pTextHost->SetPasswordChar(lpszPasswordChar);
		Invalidate();
	}

	BOOL CEditUI2::IsDigitalMode() const
	{
		return m_pTextHost->IsDigitalMode();
	}

	void CEditUI2::SetDigitalMode(BOOL bDigital)
	{
		if (IsDigitalMode() == bDigital)
			return;
		
		m_pTextHost->SetDigitalMode(bDigital);
	}

	UINT CEditUI2::GetLimitLength() const
	{
		return m_pTextHost->GetLimitLength();
	}

	void CEditUI2::SetLimitLength(UINT nLimitText)
	{
		if (GetLimitLength() == nLimitText)
			return;
		m_pTextHost->SetLimitLength(nLimitText);
	}

	void CEditUI2::SetSelectBkColor(DWORD dwSelectBkColor)
	{
		m_pTextHost->SetSelectBkColor(dwSelectBkColor);
		Invalidate();
	}

	void CEditUI2::SetCaretWidth(int nCaretWidth)
	{
		m_pTextHost->SetCaretWidth(nCaretWidth);
		Invalidate();
	}

	void CEditUI2::SetSelectTextColor(DWORD dwSelectTextColor)
	{
		m_pTextHost->SetSelectTextColor(dwSelectTextColor);
		Invalidate();
	}

	void CEditUI2::SetDisabledImage(LPCTSTR lpszDisabledImage)
	{
		if (m_sDisabledImage == lpszDisabledImage)
			return;
		m_sDisabledImage = lpszDisabledImage;
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

	void CEditUI2::SetWaterTextColor(DWORD dwWaterColor)
	{
		m_pTextHost->SetWaterTextColor(dwWaterColor);
		Invalidate();
	}

	void CEditUI2::SetWaterText(LPCTSTR lpszWaterText)
	{
		SetWaterMode(TRUE);
		m_pTextHost->SetWaterText(lpszWaterText);
		
		Invalidate();
	}

	BOOL CEditUI2::CanPaste() const
	{
		return m_pTextHost->CanPaste();
	}

	UINT CEditUI2::GetTextState() const
	{
		return m_pTextHost->GetTextState();
	}

	BOOL CEditUI2::IsWaterMode() const
	{
		return m_pTextHost->IsWaterMode();
	}

	void CEditUI2::SetWaterMode(BOOL bWaterMode)
	{
		if (IsWaterMode() == bWaterMode)
			return;

		m_pTextHost->SetWaterMode(bWaterMode);

		//m_sText = bWaterMode && IsFocused() == false ? m_sWaterText : m_sTextBak;
	}

	void CEditUI2::SetText(LPCTSTR pstrText)
	{
		if (pstrText == NULL || m_sText == pstrText)
			return;

		m_pTextHost->SetText(pstrText);
		Invalidate();
	}

	void CEditUI2::SetPos(RECT rc)
	{
		CLabelUI::SetPos(rc);
	}
}
