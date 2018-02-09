#include "StdAfx.h"
#include "UINavigateBar.h"

namespace DuiLib
{
	CNavigateBarUI::CNavigateBarUI()
	{
		m_bClicked = FALSE;
		m_pNavigateBar = new CEditUI2;
		Add(m_pNavigateBar);
		m_pNavigateBar->OnEvent += MakeDelegate(this, &CNavigateBarUI::EventNotify);
		m_pNavigateBar->SetVisible(FALSE);
	}

	CNavigateBarUI::~CNavigateBarUI()
	{

	}

	LPCTSTR CNavigateBarUI::GetClass() const
	{
		return _T("NavigateBarUI");
	}

	LPVOID CNavigateBarUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("NavigateBar")) == 0 ) return static_cast<CNavigateBarUI*>(this);
		return 
			CHorizontalLayoutUI::GetInterface(pstrName);
	}

	void CNavigateBarUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_SETFOCUS)
		{
			TCHAR szModuleName[MAX_PATH];
			GetModuleFileName(NULL,szModuleName,_countof(szModuleName));
			PathRemoveFileSpec(szModuleName);
			SetNavigatePath(szModuleName);//m_pNavigateBar->SetText(szModuleName);
			
			SwitchInternVisible(true);
			m_pNavigateBar->SetFocus();
			m_bClicked = TRUE;
			Invalidate();
		}
		else
			CHorizontalLayoutUI::DoEvent(event);
	}

	bool CNavigateBarUI::EventNotify(LPVOID lParam)
	{
		TEventUI* pEvent = (TEventUI*)lParam;
		if (pEvent->Type == UIEVENT_KILLFOCUS)
		{
			SwitchInternVisible(false);
			Invalidate();
		}

		return true;
	}

	void CNavigateBarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("font")) == 0) SetFont(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("navigatepath")) == 0) SetNavigatePath(pstrValue);
		else
			CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CNavigateBarUI::SetFont(int nFont)
	{
		m_pNavigateBar->SetFont(nFont);
	}

	void CNavigateBarUI::SetNavigatePath(LPCTSTR pstrValue)
	{
		if (m_pNavigateBar->GetText() == pstrValue)
			return;

		m_pNavigateBar->SetText(pstrValue);
		//	解析并生成新的导航路径
		PraseItemNode();
		//	刷新界面重新显示
		Invalidate();
	}

	void CNavigateBarUI::PraseItemNode()
	{
		
	}

	void CNavigateBarUI::SwitchInternVisible(bool bVisible /*= true*/)
	{
		m_pNavigateBar->SetVisible(bVisible);
		for (int n=1; n<m_items.GetSize();++n)
		{
			CControlUI* pControl = (CControlUI*)m_items[n];
			pControl->SetVisible(bVisible == false);
		}
	}
}