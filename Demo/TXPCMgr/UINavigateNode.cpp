#include "StdAfx.h"
#include "UINavigateNode.h"

namespace DuiLib
{
	CNavigateNodeUI::CNavigateNodeUI()
	{
		m_uButtonState = 0;
	}

	CNavigateNodeUI::~CNavigateNodeUI()
	{

	}

	LPCTSTR CNavigateNodeUI::GetClass() const
	{
		return _T("NavigateNodeUI");
	}

	LPVOID CNavigateNodeUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName,_T("NavigateNode")) == 0) return static_cast<CNavigateNodeUI*>(this);
		else
			return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	bool CNavigateNodeUI::Add(CControlUI* pControl)
	{
		if (pControl->GetInterface(DUI_CTR_BUTTON))
			pControl->OnEvent += MakeDelegate(this,&CNavigateNodeUI::OnButtonEvent);
		return CHorizontalLayoutUI::Add(pControl);
	}

	void CNavigateNodeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else
			CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CNavigateNodeUI::SetNormalImage(LPCTSTR lpszNormalImage)
	{
		if (m_sNormalImage == lpszNormalImage)
			return;

		m_sNormalImage = lpszNormalImage;
		Invalidate();
	}

	void CNavigateNodeUI::SetHotImage(LPCTSTR lpszHotImage)
	{
		if (m_sHotImage == lpszHotImage)
			return;

		m_sHotImage = lpszHotImage;
		Invalidate();
	}

	void CNavigateNodeUI::PaintStatusImage(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ){
					m_sHotImage.Empty();
				}
			}
			return;
		}

		if ( !m_sNormalImage.IsEmpty()) { 
			if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) {
				m_sNormalImage.Empty();
			}
		}
	}

	SIZE CNavigateNodeUI::EstimateSize(SIZE szAvailable)
	{
		if (m_items.GetSize() == 0)
			return CHorizontalLayoutUI::EstimateSize(szAvailable);

		m_cxyFixed.cx = 0;
		for (int n=0; n<m_items.GetSize(); ++n)
		{
			CControlUI* pControl = (CControlUI*)m_items[n];
			SIZE szControl = pControl->EstimateSize(szAvailable);
			m_cxyFixed.cx += szControl.cx;
		}

		if (m_cxyFixed.cx == 0)
			return CHorizontalLayoutUI::EstimateSize(szAvailable);
		else
			return CSize(m_cxyFixed.cx, szAvailable.cy);
	}

	bool CNavigateNodeUI::OnButtonEvent(LPVOID lParam)
	{
		TEventUI* pEvent = (TEventUI*)lParam;
		if (pEvent->Type == UIEVENT_MOUSEENTER) {
			if (IsEnabled()){
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}		
		} else if (pEvent->Type == UIEVENT_MOUSELEAVE) {
			if (IsEnabled()) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}		
		}
		return true;
	}
}