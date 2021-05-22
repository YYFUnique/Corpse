#include "StdAfx.h"
#include "WinMTR.h"
#include "Resource.h"

CWinMTR::CWinMTR()
{
	AddVirtualWnd(VIRTUAL_WND_IPv4, &m_IPv4Mgr);
	m_IPv4Mgr.SetVirtualWnd(this, &m_PaintManager);
}

CWinMTR::~CWinMTR()
{
	RemoveVirtualWnd(VIRTUAL_WND_IPv4);
	m_IPv4Mgr.ClearVirtualWnd(this);
}

void CWinMTR::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CWinMTR::GetWindowClassName() const
{
	return _T("WinMTR");
}

UILIB_RESOURCETYPE CWinMTR::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CWinMTR::GetZIPFileName() const
{
	return _T("WinMTR.zip");
}

CDuiString CWinMTR::GetSkinFile()
{
	return _T("WinMTR.xml");
}

CDuiString CWinMTR::GetSkinFolder()
{
	return _T("WinMTR");
}

DUI_BEGIN_MESSAGE_MAP(CWinMTR, CNotifyPump)  
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	/*DUI_ON_MSGTYPE(DUI_MSGTYPE_TIMER, OnTimer)*/
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnItemSelect)
DUI_END_MESSAGE_MAP()  

void CWinMTR::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("TabSwitch"));
	if (pList)
		pList->SelectItem(0, FALSE);

	CEditUI2* pAddr = (CEditUI2*)m_PaintManager.FindControl(_T("Addr"));
	if (pAddr != NULL)
		pAddr->SetFocus();
}

void CWinMTR::OnItemSelect(TNotifyUI& msg)
{
	if (msg.pSender == (CControlUI*)m_PaintManager.FindControl(_T("TabSwitch")))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("TabWizard"));
		if (pTabLayout == NULL)
			return;
		
		CListUI* pList = (CListUI*)msg.pSender->GetInterface(DUI_CTR_LIST);
		if (pList == NULL)
			return;

		int nSel = pList->GetCurSel();
		pTabLayout->SelectItem(nSel);

		CControlUI* pMsgNotify = GetViewObject(pTabLayout, nSel);
		if (pMsgNotify)
			m_PaintManager.SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
	}
}

CControlUI* CWinMTR::GetViewObject(CTabLayoutUI* pTabLayout,int nIndex)
{
	// 获取第一层TabLayoutUI 控件中的节点
	CVerticalLayoutUI* pSubObject = (CVerticalLayoutUI*)pTabLayout->GetItemAt(nIndex);
	if (pSubObject == NULL)
		return NULL;

	// 获取第二层的TabLayoutUI
	CTabLayoutUI* pSubTab = (CTabLayoutUI*)pSubObject->FindSubControlByClassName(_T("TabLayoutUI"));
	if (pSubTab == NULL )
		return NULL;

	// 返回当前界面展示的页面
	int nSelectId = pSubTab->GetCurSel();
	return pSubTab->GetItemAt(nSelectId);
}

void CWinMTR::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
		Close(IDOK);
	else if (msg.pSender->GetName() == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

LRESULT CWinMTR::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	PostQuitMessage(0);
	return FALSE;
}

LRESULT CWinMTR::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT bRes = FALSE;
	bHandled = TRUE;
	switch(uMsg)
	{
		case WM_NOTIFY_TAB_CHANGE:
				bRes = OnTabChange(wParam, lParam);
			break;
		default:
			bHandled = FALSE;
	}

	if (bHandled) return bRes;

	return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
}

LRESULT CWinMTR::OnTabChange(WPARAM wParam, LPARAM lParam)
{
	NTCHDR* pNTCHDR = (NTCHDR*)lParam;

	CHorizontalLayoutUI* pTabSwitch = (CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("TabSwitch"));
	if (pTabSwitch == NULL)
		return FALSE;

	COptionUI* pWizardId = (COptionUI*)pTabSwitch->GetItemAt(pNTCHDR->nWizardId);
	if (pWizardId == NULL)
		return FALSE;

	pWizardId->Selected(TRUE);

	if (pNTCHDR->nWizardId == WIZARD_ID_IPV4)
		m_IPv4Mgr.NotifyTask(pNTCHDR);
	/*else if (pNTCHDR->nWizardId == WIZARD_ID_NET)
		m_NetworkMgr.NotifyTask(pNTCHDR);*/

	return TRUE;
}