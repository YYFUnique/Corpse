#include "StdAfx.h"
#include "AIToolKit.h"
#include "Resource.h"
#include "DllCore/Utils/TextTools.h"

#define		TIMER_PCHUNTER_ID			0x1000
#define		WM_TRAYICON					WM_USER+0x1000

CAIToolkit::CAIToolkit()
{
	AddVirtualWnd(VIRTUAL_WND_KSP, &m_KSPMgr);
	m_KSPMgr.SetVirtualWnd(this, &m_PaintManager);

	AddVirtualWnd(VIRTUAL_WND_INTERFACE, &m_ISPSMgr);
	m_ISPSMgr.SetVirtualWnd(this, &m_PaintManager);
}

CAIToolkit::~CAIToolkit()
{
	RemoveVirtualWnd(VIRTUAL_WND_KSP);
	RemoveVirtualWnd(VIRTUAL_WND_INTERFACE);
}

DUI_BEGIN_MESSAGE_MAP(CAIToolkit, CNotifyPump)  
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_TIMER, OnTimer)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()  

void CAIToolkit::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CAIToolkit::GetWindowClassName() const
{
	return _T("AsiaSecurityInfo");
}

UILIB_RESOURCETYPE CAIToolkit::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

CDuiString CAIToolkit::GetZIPFileName() const
{
	return _T("AIToolKit.zip");
}

CDuiString CAIToolkit::GetSkinFile()
{
	return _T("AIToolKit.xml");
}

CDuiString CAIToolkit::GetSkinFolder()
{
	return _T("AIToolKit");
}

void CAIToolkit::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

void CAIToolkit::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
	{
		Close(IDOK);
	}
	else if (msg.pSender->GetName() == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CAIToolkit::OnTimer(TNotifyUI& msg)
{
	if (msg.wParam == TIMER_PCHUNTER_ID) {
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("TabWizard"));
		if (pTabLayout == NULL)
			return;

		CControlUI* pMsgNotify = GetViewObject(pTabLayout, m_dwSelectOption);
		if (pMsgNotify)
			m_PaintManager.SendNotify(pMsgNotify, DUI_MSGTYPE_TIMEREX);
	}
}

CControlUI* CAIToolkit::GetViewObject(CTabLayoutUI* pTabLayout,int nIndex)
{
	// 获取第一层TabLayoutUI 控件中的节点
	CVerticalLayoutUI* pSubObject = (CVerticalLayoutUI*)pTabLayout->GetItemAt(nIndex);
	// 获取第二层的TabLayoutUI
	CTabLayoutUI* pSubTab = (CTabLayoutUI*)pSubObject->FindSubControlByClassName(_T("TabLayoutUI"));
	if (pSubTab == NULL )
		return NULL;

	// 返回当前界面展示的页面
	int nSelectId = pSubTab->GetCurSel();
	return pSubTab->GetItemAt(nSelectId);
}

void CAIToolkit::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetParent() == (CControlUI*)m_PaintManager.FindControl(_T("TabSwitch")))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("TabWizard"));
		if (pTabLayout == NULL)
			return;

		m_dwSelectOption = _ttoi(msg.pSender->GetUserData());
		pTabLayout->SelectItem(m_dwSelectOption);

		CControlUI* pMsgNotify = GetViewObject(pTabLayout, m_dwSelectOption);
		if (pMsgNotify)
			m_PaintManager.SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
	}
}

LRESULT CAIToolkit::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_dwSelectOption == WIZARD_ID_KSP)
	{
		if (m_KSPMgr.HandleMessage(uMsg, wParam, lParam) != FALSE)
			return 0;
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CAIToolkit::OnClose(UINT , WPARAM , LPARAM , BOOL& bHandled)
{
	bHandled = TRUE;
	PostQuitMessage(0);
	return TRUE;
}

LRESULT CAIToolkit::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	LRESULT lRes = FALSE;
	if (wParam == VK_F5)
		lRes =  OnF5Down(uMsg, wParam, lParam, bHandled);
	else
		lRes =  __super::OnKeyDown(uMsg, wParam, lParam, bHandled);

	return lRes;
}

LRESULT CAIToolkit::OnF5Down(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	do 
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("TabWizard"));
		if (pTabLayout == NULL)
			break;

		UINT nIndex = pTabLayout->GetCurSel();

		CControlUI* pVirtualWnd = GetViewObject(pTabLayout,nIndex);
		if (pVirtualWnd == NULL)
			break;

		m_PaintManager.SendNotify(pVirtualWnd,DUI_MSGTYPE_REFRESH);
	} while (FALSE);

	return TRUE;
}