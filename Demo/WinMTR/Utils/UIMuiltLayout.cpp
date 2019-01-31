#include "StdAfx.h"
#include "UIMuiltLayout.h"

CMuiltLayoutUI::CMuiltLayoutUI()
{
	m_pHeader = new CHorizontalLayoutUI;
	CContainerUI::Add(m_pHeader);
	m_pBody = new CTabLayoutUI;
	CContainerUI::Add(m_pBody);
}

CMuiltLayoutUI::~CMuiltLayoutUI()
{
	
}

LPCTSTR CMuiltLayoutUI::GetClass() const
{
	return _T("MuiltLayoutUI");
}

LPVOID CMuiltLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, DUI_CTR_IWONER) == 0)
		return static_cast<IOwner*>(this);
	else if (_tcsicmp(pstrName, DUI_CTR_MUILTLAYOUT) == 0)
		return static_cast<CMuiltLayoutUI*>(this);
	else
		return CContainerUI::GetInterface(pstrName);
}

void CMuiltLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("selectedid")) == 0 ) 
		SelectItem(_ttoi(pstrValue));
	else
		CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CMuiltLayoutUI::SetText(int iIndex, LPCTSTR lpszText)
{
	CControlUI* pControl = m_pHeader->GetItemAt(iIndex);
	if (pControl != NULL)
		pControl->SetText(lpszText);
}

bool CMuiltLayoutUI::Add(CControlUI* pControl)
{
	bool bSuccess = false;
	// 如果是容器控件，将存放与TabLayout布局中
	CContainerUI* pContainer = (CContainerUI*)pControl->GetInterface(DUI_CTR_CONTAINER);
	if (pContainer != NULL)
	{
		bSuccess = m_pBody->Add(pControl);
		CTabOptionUI* pTabOption = new CTabOptionUI;
		if (pTabOption != NULL)
		{
			pTabOption->SetOwner(this);
			m_pHeader->Add(pTabOption);
		}
	}

	return bSuccess;
}

bool CMuiltLayoutUI::AddAt(CControlUI* pControl, int iIndex)
{
	bool bSuccess = false;
	// 如果是容器控件，将存放与TabLayout布局中
	CContainerUI* pContainer = (CContainerUI*)pControl->GetInterface(DUI_CTR_CONTAINER);
	if (pContainer != NULL)
	{
		bSuccess = m_pBody->AddAt(pControl, iIndex);
		CTabOptionUI* pTabOption = new CTabOptionUI;
		if (pTabOption != NULL)
		{
			pTabOption->SetOwner(this);
			m_pHeader->AddAt(pTabOption, iIndex);
		}
	}

	return bSuccess;
}

bool CMuiltLayoutUI::Remove(CControlUI* pControl)
{
	bool bSuccess = false;
	CContainerUI* pContainer = (CContainerUI*)pControl->GetInterface(DUI_CTR_CONTAINER);
	if (pContainer != NULL)
	{
		int iIndex = m_pBody->GetItemIndex(pControl);
		bSuccess = m_pBody->Remove(pControl);
		if (bSuccess != false)
		{
			CControlUI* pControl = m_pHeader->GetItemAt(iIndex);
			m_pHeader->Remove(pControl);
		}
	}

	return bSuccess;
}

void CMuiltLayoutUI::RemoveAll()
{
	m_pBody->RemoveAll();
	m_pHeader->RemoveAll();
}

int CMuiltLayoutUI::GetCurSel() const
{
	return m_pBody->GetCurSel();
}

bool CMuiltLayoutUI::SelectItem(int iIndex)
{
	return m_pBody->SelectItem(iIndex);
}

bool CMuiltLayoutUI::SelectItem(CControlUI* pControl)
{
	return m_pBody->SelectItem(pControl);
}

void CMuiltLayoutUI::Notify(LPCTSTR lpszNotify)
{

}