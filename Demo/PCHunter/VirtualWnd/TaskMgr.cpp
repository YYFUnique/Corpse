#include "stdafx.h"

CTaskMgr::CTaskMgr()
{
	AddVirtualWnd(_T("app"),&m_App);
	AddVirtualWnd(_T("process"),&m_Process);
	AddVirtualWnd(_T("services"),&m_Services);
	AddVirtualWnd(_T("driver"),&m_CoreDriver);
	AddVirtualWnd(_T("user"),&m_UserInfo);
}

CTaskMgr::~CTaskMgr()
{
	RemoveVirtualWnd(_T("app"));
	RemoveVirtualWnd(_T("process"));
	RemoveVirtualWnd(_T("services"));
	RemoveVirtualWnd(_T("driver"));
	RemoveVirtualWnd(_T("user"));
}

DUI_BEGIN_MESSAGE_MAP(CTaskMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
DUI_END_MESSAGE_MAP()

//void CTaskMgr::SetPaintMagager(CPaintManagerUI* pPaintMgr)
//{
//	CVirtualWnd::SetPaintMagager(pPaintMgr);
//	CBase::SetPaintManager(m_pPaintManager);
//}

void CTaskMgr::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetParent()->GetName() == _T("VLayoutTask"))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("TaskWizard"));
		if (pTabLayout != NULL)
			pTabLayout->SelectItem(_ttoi(msg.pSender->GetUserData()));
	}
}

void CTaskMgr::OnApplication(CControlUI* pControl)
{
	m_App.OnAppMenu(pControl);
}