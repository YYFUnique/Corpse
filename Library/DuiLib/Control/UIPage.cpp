#include "StdAfx.h"
#include "UIPage.h"

namespace DuiLib
{
	CPageUI::CPageUI()
		:m_pList(NULL)
	{
		m_nCount = 10;
		m_nPage = 1;
		//m_nTotalPage = m_pOwner->GetCount();
		
		m_pFirstPage = new CButtonUI;
		m_pLastPage = new CButtonUI;
		m_pPrePage = new CButtonUI;
		m_pNextPage = new CButtonUI;
		m_pGoto = new CButtonUI;

		m_pFirstPage->SetText(_T("首页"));
		m_pLastPage->SetText(_T("尾页"));
		m_pPrePage->SetText(_T("上一页"));
		m_pNextPage->SetText(_T("下一页"));
		m_pGoto->SetText(_T("跳转"));

		Add(m_pFirstPage);
		Add(m_pLastPage);
		Add(m_pPrePage);
		Add(m_pNextPage);
		Add(m_pGoto);

		m_pFirstPage->OnNotify += MakeDelegate(this, &CPageUI::Notify);
		m_pLastPage->OnNotify += MakeDelegate(this, &CPageUI::Notify);
		m_pPrePage->OnNotify += MakeDelegate(this, &CPageUI::Notify);
		m_pNextPage->OnNotify += MakeDelegate(this, &CPageUI::Notify);
		m_pGoto->OnNotify += MakeDelegate(this, &CPageUI::Notify);
	}

	LPCTSTR CPageUI::GetClass() const
	{
		return _T("PageUI");
	}

	UINT CPageUI::GetControlFlags() const
	{
		return UIFLAG_TABSTOP;
	}

	LPVOID CPageUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_PAGE) == 0 ) return static_cast<CPageUI*>(this);
		else
			return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	bool CPageUI::Notify(LPVOID lParam)
	{
		TNotifyUI* pNotify = (TNotifyUI*)lParam;
		if (pNotify->sType == DUI_MSGTYPE_CLICK)
		{
			if (pNotify->pSender == m_pFirstPage)
				SetFirstPage();
			else if (pNotify->pSender == m_pLastPage)
				SetLastPage();
			else if (pNotify->pSender == m_pPrePage)
				SetPrePage();
			else if (pNotify->pSender == m_pNextPage)
				SetNextPage();
			else if (pNotify->pSender == m_pGoto)
				SetPageTo(0);
		}
	
		return false;
	}

	void CPageUI::DoEvent(TEventUI& event)
	{
		CHorizontalLayoutUI::DoEvent(event);
	}

	void CPageUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("count")) == 0) SetCount(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("page")) == 0) SetPageTo(_ttoi(pstrValue));
		else
			CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CPageUI::SetCount(int nCount)
	{
		m_nCount = nCount;
	}

	void CPageUI::SetTotal(int nTotal)
	{
		m_nTotal = nTotal;	
	}

	void CPageUI::SetPage(CListBodyUI* pList)
	{
		m_pList = pList;
	}

	void CPageUI::SetFirstPage()
	{
		//m_pList->SetFirstPage();
	}

	void CPageUI::SetLastPage()
	{
		//m_pList->SetLastPage();
	}

	void CPageUI::SetPrePage()
	{
		//m_pList->SetPrePage();
	}

	void CPageUI::SetNextPage()
	{
		//m_pList->SetNextPage();
	}

	void CPageUI::SetPageTo(int nPage)
	{
		if (m_nPage != nPage)
			return;

		m_nPage = nPage;
		//m_pList->SetPageTo(m_nPage);
	}
}