#include "StdAfx.h"
#include "FileProcMgr.h"

CFileProcMgr::CFileProcMgr()
{
	m_pPaintManager = NULL;
}

CFileProcMgr::~CFileProcMgr()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CFileProcMgr, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CFileProcMgr::ClearVirtualWnd(CNotifyPump* pNotifyPump)
{
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_WINHEX);
	pNotifyPump->RemoveVirtualWnd(VIRTUAL_WND_FILE_HIDE);
}

void CFileProcMgr::SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager)
{
	m_pPaintManager = pPaintManager;

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_FILE_HIDE, &m_HideFile);
	m_HideFile.SetPaintMagager(pPaintManager);

	pNotifyPump->AddVirtualWnd(VIRTUAL_WND_WINHEX, &m_WinHex);
	m_WinHex.SetPaintMagager(pPaintManager);
}

void CFileProcMgr::OnSelectChanged( TNotifyUI &msg )
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

void CFileProcMgr::OnAppMenu(CControlUI* pControl)
{
	m_HideFile.OnAppMenu(pControl);
}

//void CFileProcMgr::OnServiceMenu(CControlUI* pControl)
//{
//	m_Service.OnServiceMenu(pControl);
//}