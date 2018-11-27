#include "stdafx.h"
#include "UIComboBox.h"

namespace DuiLib
{
	CComboBoxUI::CComboBoxUI()
		: m_nArrowWidth(20)
	{
		m_pTextHost = new CEditTextHost;
	}

	CComboBoxUI::~CComboBoxUI()
	{
		if (m_pTextHost != NULL)
			delete m_pTextHost;
	}

	LPCTSTR CComboBoxUI::GetClass() const
	{
		return _T("ComboBoxUI");
	}

	LPVOID CComboBoxUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_COMBOBOX) == 0)
			return static_cast<CComboBoxUI*>(this);
		else
			return CComboUI::GetInterface(pstrName);
	}

	UINT CComboBoxUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return CComboUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP | UIFLAG_IME_COMPOSITION;
	}

	void CComboBoxUI::DoInit()
	{
		m_pTextHost->SetOwner(this);

		m_pTextHost->SetTextStyle(m_ListInfo.uTextStyle);
		m_pTextHost->SetFont(m_ListInfo.nFont);
		m_pTextHost->SetTextColor(m_ListInfo.dwTextColor);
		m_pTextHost->SetDisabledTextColor(m_ListInfo.dwDisabledTextColor);
		// 预留右边下拉箭头宽度
		m_rcTextPadding.right = m_nArrowWidth;
		m_pTextHost->SetTextPadding(m_rcTextPadding);
		m_pTextHost->SetCaretColor(m_dwCaretColor);

		m_pTextHost->SetText(CComboUI::GetText());
	}

	void CComboBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName,_T("caretwidth")) == 0) SetCaretWidth(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("arrowwidth")) == 0) SetArrowWidth(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("digital")) == 0) SetDigitalMode(_tcsicmp(pstrValue,_T("true")) == 0);
		else if (_tcsicmp(pstrName, _T("limited")) == 0) SetLimitLength(_ttoi(pstrValue));
		else
			CComboUI::SetAttribute(pstrName, pstrValue);
	}

	void CComboBoxUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CComboUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_BUTTONDOWN)
		{
			// 没有点击到文本内容中，由控件自己处理（点击到了下拉箭头）
			RECT rcArrow = m_rcItem;
			rcArrow.left = rcArrow.right - m_nArrowWidth;
			if (PtInRect(&rcArrow, event.ptMouse))
			{
				CComboUI::DoEvent(event);
				return;
			}
		} else if (event.Type == UIEVENT_KEYDOWN) {
			// 用于控制在编辑模式下，按上下方向键选择内容
			switch(event.chKey)
			{
				case VK_UP:
						SelectItem(FindSelectable(m_iCurSel - 1, false));
					return;
				case VK_DOWN:
						SelectItem(FindSelectable(m_iCurSel + 1, true));
					return;
			}
		} else if (event.Type == UIEVENT_KILLFOCUS) {
			// 发送KILLFOCUS的控件为空
			if (event.pSender == NULL)
				return;
		}
		
		if (m_pTextHost->DoEvent(event) != FALSE)
			return;

		if (event.Type == UIEVENT_SETCURSOR)
		{
			// 修改界面显示的鼠标指针
			RECT rcArrow = m_rcItem;
			rcArrow.left = rcArrow.right - m_nArrowWidth;
			if (PtInRect(&rcArrow, event.ptMouse))
				m_CursorType = IDC_ARROW;
			else
				m_CursorType = IDC_IBEAM;
			CComboUI::DoEvent(event);
		} else 
			CComboUI::DoEvent(event);
	}

	CDuiString CComboBoxUI::GetText() const
	{
		return m_pTextHost->GetText();
	}

	bool CComboBoxUI::SelectItem(int iIndex, bool bTakeFocus)
	{
		bool bRet = CComboUI::SelectItem(iIndex, bTakeFocus);

		m_pTextHost->SetText(CComboUI::GetText());
		m_pTextHost->SelectAll();

		return bRet;
	}

	void CComboBoxUI::SetCaretWidth(UINT nCaretWidth)
	{
		m_pTextHost->SetCaretWidth(nCaretWidth);
		Invalidate();
	}

	void CComboBoxUI::SetArrowWidth(UINT nArrowWidth)
	{
		if (m_nArrowWidth == nArrowWidth)
			return;

		m_nArrowWidth = nArrowWidth;
		Invalidate();
	}

	void CComboBoxUI::SetDigitalMode(BOOL bDigital)
	{
		m_pTextHost->SetDigitalMode(bDigital);
	}

	void CComboBoxUI::SetLimitLength(UINT nLimitLen)
	{
		m_pTextHost->SetLimitLength(nLimitLen);
	}

	void CComboBoxUI::PaintStatusImage(HDC hDC)
	{
		m_pTextHost->PaintStatusImage(hDC);
		UINT nButtonState = m_pTextHost->GetTextState();

		// 按钮和编辑器状态暂时不区分
		if( (nButtonState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
				else return;
			}
		}
		else if( (nButtonState & UISTATE_PUSHED) != 0 ) {
			if( !m_sPushedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sPushedImage) ) m_sPushedImage.Empty();
				else return;
			}
		}
		else if( (nButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
				else return;
			}
		}
		else if( (nButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else return;
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
			else return;
		}
	}

	void CComboBoxUI::PaintText(HDC hDC)
	{
		m_pTextHost->PaintText(hDC);
	}

	void CComboBoxUI::PaintBorder(HDC hDC)
	{
		m_pTextHost->PaintCaret(hDC);
	}
}
