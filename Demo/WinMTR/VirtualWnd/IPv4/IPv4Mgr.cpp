#include "StdAfx.h"
#include "IPv4Mgr.h"

CIPv4Mgr::CIPv4Mgr()
{
	m_pNotifyPump = NULL;
	m_pPaintManager = NULL;
}

CIPv4Mgr::~CIPv4Mgr()
{
	m_pNotifyPump = NULL;
	m_pPaintManager = NULL;

	m_ResultView.Empty();
}

DUI_BEGIN_MESSAGE_MAP(CIPv4Mgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CIPv4Mgr::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_IPv4Param);
	for (int n=0; n<m_ResultView.GetSize(); ++n)
	{
		CIPv4Result* pIPv4Result = (CIPv4Result*)m_ResultView.GetAt(n);
		if (pIPv4Result == NULL)
			continue;

		CDuiString strVirtualWnd = pIPv4Result->GetVirtualWnd();
		pNotifyPump->RemoveVirtualWnd(strVirtualWnd);
		delete pIPv4Result;
	}
}

void CIPv4Mgr::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;	

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_IPv4Param, &m_IPv4Param);
	m_IPv4Param.SetPaintMagager(pPaintManager);
	m_IPv4Param.SetOwner(this);

	m_pNotifyPump = pNotifyPump;
}

void CIPv4Mgr::AddVirtualWnd(LPCTSTR lpszVirtualWnd)
{
	CIPv4Result* pIPv4Result = new CIPv4Result;
	if (pIPv4Result == NULL)
		return;

	pIPv4Result->SetPaintMagager(m_pPaintManager);
	pIPv4Result->SetVirtualWnd(lpszVirtualWnd);

	if (m_pNotifyPump)
		m_pNotifyPump->AddVirtualWnd(lpszVirtualWnd, pIPv4Result);

	m_ResultView.Add(pIPv4Result);
}

void CIPv4Mgr::OnSelectChanged(TNotifyUI &msg)
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("IPv4Wizard"));
	if (pTabLayout == NULL)
		return;

	UINT nIndex = _ttoi(msg.pSender->GetUserData());
	CControlUI* pMsgNotify = pTabLayout->GetItemAt(nIndex);
	pTabLayout->SelectItem(nIndex);

	if (pMsgNotify)
		m_pPaintManager->SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
}

void CIPv4Mgr::NotifyTask(PCNTCHDR pNTCHDR)
{
	CVerticalLayoutUI* pVLayoutTask = (CVerticalLayoutUI*)m_pPaintManager->FindControl(_T("VLayoutIPv4"));
	if (pVLayoutTask == NULL)
		return;

	// 设置参数
	for (int n=0; n< m_ResultView.GetSize(); ++n)
	{
		CIPv4Result* pIPv4Result = (CIPv4Result*)m_ResultView.GetAt(n);
		if (pIPv4Result == NULL)
			continue;
		if (_tcsicmp(pIPv4Result->GetVirtualWnd(), pNTCHDR->strTabTo) == 0)
			pIPv4Result->NotifyTask(pNTCHDR);		
	}

	// 枚举OptionUI按钮
	UINT nIndex = GetTabViewIndex(pNTCHDR->strTabTo);
	COptionUI* pTabWizard = (COptionUI*)pVLayoutTask->GetItemAt(2 * nIndex + 1);	// 中间多一个Control控件
	if (pTabWizard == NULL)
		return;

	pTabWizard->Selected(TRUE);
}

UINT CIPv4Mgr::GetTabViewIndex(LPCTSTR lpszTabName) const
{
	CStdArray strVirtualName;
	strVirtualName.Add(VIRTUAL_WND_IPv4Param);
	for (int n=0; n< m_ResultView.GetSize(); ++n)
	{
		CIPv4Result* pIPv4Result = (CIPv4Result*)m_ResultView.GetAt(n);
		if (pIPv4Result)
			strVirtualName.Add(pIPv4Result->GetVirtualWnd());
	}

	for (int n=0; n<strVirtualName.GetCount(); ++n)
	{
		if (strVirtualName.GetAt(n).CompareNoCase(lpszTabName) == 0)
			return n;
	}

	return 0;
}