#include "StdAfx.h"
#include "AIToolKit.h"
#include "Resource.h"
#include "DllCore/Utils/TextTools.h"
#include "Utils/ControlBuilder.h"

#include "libModule/UIClient.h"

#define		TIMER_PCHUNTER_ID			0x1000
#define		WM_TRAYICON					WM_USER+0x1000

CAIToolkit::CAIToolkit()
{
	m_pUIPlatform = NULL;
	AddVirtualWnd(VIRTUAL_WND_DSAGENT, &m_DSMgr);
	m_DSMgr.SetVirtualWnd(this, &m_PaintManager);

	AddVirtualWnd(VIRTUAL_WND_KSP, &m_KSPMgr);
	m_KSPMgr.SetVirtualWnd(this, &m_PaintManager);

	AddVirtualWnd(VIRTUAL_WND_INTERFACE, &m_ISPSMgr);
	m_ISPSMgr.SetVirtualWnd(this, &m_PaintManager);
}

CAIToolkit::~CAIToolkit()
{
	if (m_pUIPlatform != NULL)
	{
		delete m_pUIPlatform;
		m_pUIPlatform = NULL;
	}
	RemoveVirtualWnd(VIRTUAL_WND_DSAGENT);
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

CControlUI* CAIToolkit::CreateControl(LPCTSTR pstrClass)
{
	CControlBuilder RootBuilder;
	return RootBuilder.CreateControl(pstrClass);
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
#ifdef _DEBUG
	return _T("AIToolKit");
#else
	return _T("");
#endif
}

void CAIToolkit::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	m_pUIPlatform = new UIPlatform(_T("AiToolkit"));
	if (m_pUIPlatform->RunAsync() == FALSE)
	{
		delete m_pUIPlatform;
		m_pUIPlatform = NULL;
	}


	UIClient client(*m_pUIPlatform);
	client.IsUIModuleInit();

	CHorizontalLayoutUI* pControl = (CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("TabSwitch"));
	if (pControl)
		m_PaintManager.SendNotify(pControl->GetItemAt(0), DUI_MSGTYPE_SELECTCHANGED);
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

LRESULT CAIToolkit::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT bRes = FALSE;
	bHandled = TRUE;
	switch(uMsg)
	{
	case WM_MENUCLICK:
		bRes = OnMenuClick(wParam, lParam);
		break;
	default:
		bHandled = FALSE;
	}

	if (bHandled) return bRes;

	return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
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

LRESULT CAIToolkit::OnMenuClick(WPARAM wParam, LPARAM lParam)
{
	CControlUI* pControl = (CControlUI*)lParam;
	ASSERT(pControl);
	if (pControl == NULL)
		return FALSE;

	//获取菜单对应根节点的控件名称
	CDuiString strMenuName = pControl->GetManager()->GetRoot()->GetName();
	if (strMenuName == _T("KSPMenu"))
		m_KSPMgr.OnKSPMenu(pControl);

	return TRUE;
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