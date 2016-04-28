#include "stdafx.h"

namespace DuiLib
{
	CIPAddressUI::CIPAddressUI()
		:m_nButtonState(0)
	{
		m_CursorType = IDC_IBEAM;
		for (int n=0; n<ADDRESS_IPV4; ++n)
		{
			m_pBlock[n] = new CRichEditUI;
			m_pBlock[n]->SetAttribute(_T("align"), _T("center"));
			m_pBlock[n]->SetAttribute(_T("numberonly"), _T("true"));
			m_pBlock[n]->SetAttribute(_T("rich"), _T("false"));
			m_pBlock[n]->SetAttribute(_T("multiline"), _T("false"));
			m_pBlock[n]->SetAttribute(_T("maxchar"), _T("3")); 
			m_pBlock[n]->SetAttribute(_T("font"), _T("0"));
			m_pBlock[n]->SetAttribute(_T("wanttab"),_T("true"));
			m_pBlock[n]->OnEvent += MakeDelegate(this, &CIPAddressUI::DoEvent);
		}
		//1一个IP地址控件，由4个编辑框和3个小黑点组成
		for (int n=0; n<7; ++n)
		{
			if (n%2 == 0)
				Add(m_pBlock[n/2]);
			else
			{
				CLabelUI* pDot = new CLabelUI;
				pDot->SetText(_T("."));
				pDot->SetAutoCalcWidth(true);
				pDot->SetAttribute(_T("cursor"),_T("ibeam"));
				Add(pDot);
			}
		}
	}

	LPCTSTR CIPAddressUI::GetClass() const
	{
		return _T("IPAddressUI");
	}

	UINT CIPAddressUI::GetControlFlags() const
	{
		if (IsEnabled() == FALSE)
			return 0;
		else
			return UIFLAG_TABSTOP|UIFLAG_SETCURSOR|UIFLAG_WANTRETURN;
	}

	LPVOID CIPAddressUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_IPADDRESS) == 0 )	return static_cast<CIPAddressUI*>(this);
		else
			return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	void CIPAddressUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CHorizontalLayoutUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_MOUSEENTER ){
			if( IsEnabled() ) {
				m_nButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}else if( event.Type == UIEVENT_MOUSELEAVE ){
			if( IsEnabled() ) {
				m_nButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}

		CHorizontalLayoutUI::DoEvent(event);
	}

	bool CIPAddressUI::DoEvent(LPVOID lParam)
	{
		const TEventUI* pEvent = (TEventUI*)lParam;
		if (pEvent->Type == UIEVENT_CHAR && pEvent->chKey == _T('.')){
			if (IsEnabled()){
				//如果是最后一个输入框，输入'.'就不用跳转了
				if (pEvent->pSender == m_pBlock[3])
					return false;
				
				pEvent->pSender->GetManager()->SetNextTabControl();
				CRichEditUI* pRichedit = (CRichEditUI*)GetManager()->GetFocus();
				//默认选中编辑框内容
				if (pRichedit)
					pRichedit->SetSel(0,pRichedit->GetTextLength());

				//由于切换焦点后，当前控件会失去焦点，故手动设置焦点
				m_bFocused = true;
				return false;
			}
		}
		//WM_KEYDOWN RichEdit控件默认没有转发，需要手动转发
		else if (pEvent->Type == UIEVENT_KEYDOWN )
		{	
			if (IsEnabled()){
				//如果是方向键
				if (pEvent->chKey == VK_LEFT || pEvent->chKey == VK_RIGHT){
					if ((pEvent->chKey == VK_LEFT && pEvent->pSender == m_pBlock[0]) ||
						(pEvent->chKey == VK_RIGHT && pEvent->pSender == m_pBlock[3]))
						return false;

					//判断当前是否需要切换焦点，目前以光标是否在开头或者末尾为准
					CRichEditUI* pRichedit = (CRichEditUI*)GetManager()->GetFocus();
					if (pRichedit->GetInterface(DUI_CTR_RICHEDIT) != pRichedit)
						return false;

					LONG lStart=0, lEnd=0;
					pRichedit->GetSel(lStart, lEnd);
					//获取RichEdit文本长度
					LONG lTextLen = pRichedit->GetTextLength();
					if ((pEvent->chKey == VK_LEFT && lStart != 0) ||
						(pEvent->chKey == VK_RIGHT && lEnd != lTextLen))
						return false;

					pEvent->pSender->GetManager()->SetNextTabControl(pEvent->chKey == VK_RIGHT);

					m_bFocused = true;
					return false;
				}
				//如果是回退键
				else if (pEvent->chKey == VK_BACK){
					CRichEditUI* pRichedit = (CRichEditUI*)GetManager()->GetFocus();
					if (pRichedit->GetInterface(DUI_CTR_RICHEDIT) != pRichedit)
						return false;

					LONG lStart=0, lEnd=0;
					pRichedit->GetSel(lStart, lEnd);
					if (lStart != 0)
						return false;

					pEvent->pSender->GetManager()->SetNextTabControl(pEvent->chKey == VK_RIGHT);

					m_bFocused = true;
					return false;
				}
				else if (pEvent->chKey == VK_TAB){
					BOOL bForward = (pEvent->wKeyState & MK_SHIFT) == MK_SHIFT ? TRUE : FALSE;
					do 
					{
						GetManager()->SetNextTabControl(bForward == FALSE);
					} while (GetManager()->GetFocus()->GetParent() == this);
				}
			}
		}
		else if (pEvent->Type == UIEVENT_SETFOCUS){
			m_bFocused = true;
			Invalidate();
		}
		else if (pEvent->Type == UIEVENT_KILLFOCUS){
			m_bFocused = false;
			Invalidate();
		}else if( pEvent->Type == UIEVENT_SETCURSOR && IsEnabled()){
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return false;
		}
		return true;
	}

	void CIPAddressUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_nButtonState |= UISTATE_FOCUSED;
		else m_nButtonState &= ~UISTATE_FOCUSED;
		if( !IsEnabled() ) m_nButtonState |= UISTATE_DISABLED;
		else m_nButtonState &= ~UISTATE_DISABLED;

		if( (m_nButtonState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
				else return;
			}
		}
		else if( (m_nButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else return;
			}
		}
		else if( (m_nButtonState & UISTATE_HOT) != 0 ) {
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

	void CIPAddressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName,_T("normalimage")) == 0)	SetNormalImage(pstrValue);
		else if (_tcsicmp(pstrName,_T("hotimage")) == 0)	SetHotImage(pstrValue);
		else if (_tcsicmp(pstrName,_T("focusedimage")) == 0)  SetFocusedImage(pstrValue);
		else if (_tcsicmp(pstrName,_T("disabledimage")) == 0) SetDisabledImage(pstrValue);
		else
			CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CIPAddressUI::SetText(LPCTSTR pstrText)
	{
		TCHAR szIPByte[4][10] = {0};
		_stscanf_s(pstrText, _T("%[^.].%[^.].%[^.].%s"), szIPByte[0],_countof(szIPByte[0]),
																					  szIPByte[1],_countof(szIPByte[1]),			
																					  szIPByte[2],_countof(szIPByte[2]),
																					  szIPByte[3],_countof(szIPByte[3]));


		for (int n=0; n< ADDRESS_IPV4; ++n)
			m_pBlock[n]->SetText(szIPByte[n]);
	}
	
	CDuiString CIPAddressUI::GetText() const
	{
		CDuiString strText;
		for (int n=0; n<ADDRESS_IPV4; ++n)
		{
			LPCTSTR lpszFiled = m_pBlock[n]->GetText();
			if (*lpszFiled == NULL)
				lpszFiled = _T("0");

			strText.AppendFormat(_T("%s."),lpszFiled);
		}

		return strText.TrimRight(_T("."));
	}

	void CIPAddressUI::SetNormalImage(LPCTSTR lpszNormalImage)
	{
		if (m_sNormalImage == lpszNormalImage)
			return;
		m_sNormalImage = lpszNormalImage;
		Invalidate();
	}

	void CIPAddressUI::SetHotImage(LPCTSTR lpszHotImage)
	{
		if (m_sHotImage == lpszHotImage)
			return;
		m_sHotImage = lpszHotImage;
		Invalidate();
	}

	void CIPAddressUI::SetFocusedImage(LPCTSTR lpszFocusedImage)
	{
		if (m_sFocusedImage == lpszFocusedImage)
			return;
		m_sFocusedImage = lpszFocusedImage;
		Invalidate();
	}

	void CIPAddressUI::SetDisabledImage(LPCTSTR lpszDisabledImage)
	{
		if (m_sDisabledImage == lpszDisabledImage)
			return;
		m_sDisabledImage = lpszDisabledImage;
		Invalidate();
	}
}