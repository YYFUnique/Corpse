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

void CTaskMgr::OnServiceMenu(CControlUI* pControl)
{
	m_Service.OnServiceMenu(pControl);
}