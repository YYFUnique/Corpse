#include "stdafx.h"

CSysInfoMgr::CSysInfoMgr()
{
	AddVirtualWnd(_T("osinfo"), &m_OsInfo);
}

CSysInfoMgr::~CSysInfoMgr()
{
	RemoveVirtualWnd(_T("osinfo"));
}

DUI_BEGIN_MESSAGE_MAP(CSysInfoMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CSysInfoMgr::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetParent()->GetName() == _T("VLayoutSysInfo"))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("InfoWizard"));
		if (pTabLayout != NULL)
			pTabLayout->SelectItem(_ttoi(msg.pSender->GetUserData()));
	}
}