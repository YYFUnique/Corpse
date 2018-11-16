#include "StdAfx.h"
#include "TaskMgr.h"

CTaskMgr::CTaskMgr()
{
	m_pPaintManager = NULL;
}

CTaskMgr::~CTaskMgr()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CTaskMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CTaskMgr::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_APP);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_PROCESS);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_SERVICE);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_USERINFO);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_DRIVER);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_CERT);
}

void CTaskMgr::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_APP, &m_App);
	m_App.SetPaintMagager(pPaintManager);

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_PROCESS, &m_Process);
	m_Process.SetPaintMagager(pPaintManager);

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_SERVICE, &m_Service);
	m_Service.SetPaintManager(pPaintManager);

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_USERINFO, &m_UserInfo);
	m_UserInfo.SetPaintManager(pPaintManager);

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_DRIVER, &m_CoreDriver);
	m_CoreDriver.SetPaintManager(pPaintManager);

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_CERT, &m_ViewCert);
	m_ViewCert.SetPaintManager(pPaintManager);
}

void CTaskMgr::OnSelectChanged( TNotifyUI &msg )
{
	CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("TaskWizard"));
	if (pTabLayout == NULL)
		return;
	
	UINT nIndex = _ttoi(msg.pSender->GetUserData());
	CControlUI* pMsgNotify = pTabLayout->GetItemAt(nIndex);
	pTabLayout->SelectItem(nIndex);

	if (pMsgNotify)
		m_pPaintManager->SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
}

void CTaskMgr::OnAppMenu(CControlUI* pControl)
{
	m_App.OnAppMenu(pControl);
}

void	CTaskMgr::OnProcessMenu(CControlUI* pControl)
{	
	m_Process.OnProcessMenu(pControl);
}

void CTaskMgr::OnServiceMenu(CControlUI* pControl)
{
	m_Service.OnServiceMenu(pControl);
}

void CTaskMgr::NotifyTask(PCNTCHDR pNTCHDR)
{
	CVerticalLayoutUI* pVLayoutTask = (CVerticalLayoutUI*)m_pPaintManager->FindControl(_T("VLayoutTask"));
	if (pVLayoutTask == NULL)
		return;

	COptionUI* pTabWizard = (COptionUI*)pVLayoutTask->GetItemAt(GetTabViewIndex(pNTCHDR->strTabTo));
	if (pTabWizard == NULL)
		return;
	pTabWizard->Selected(TRUE);

	if (pNTCHDR->strTabTo.CompareNoCase(VIRTUAL_WND_APP) == 0)
	{
	}
	else if (pNTCHDR->strTabTo.CompareNoCase(VIRTUAL_WND_PROCESS) == 0)
	{
		m_Process.NotifyTask(pNTCHDR);
	}
	else if (pNTCHDR->strTabTo.CompareNoCase(VIRTUAL_WND_SERVICE) == 0)
	{
		m_Service.NotifyTask(pNTCHDR);
	}
}

int CTaskMgr::GetTabViewIndex(LPCTSTR lpszTabName) const
{
	LPCTSTR lpszVirtualName[] = {VIRTUAL_WND_APP, VIRTUAL_WND_PROCESS, VIRTUAL_WND_SERVICE};
	for (int n=0; n<_countof(lpszVirtualName); ++n)
	{
		if (_tcsicmp(lpszVirtualName[n], lpszTabName) == 0)
			return n;
	}

	return 0;
}