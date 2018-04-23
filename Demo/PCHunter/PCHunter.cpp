#include "StdAfx.h"
#include "Resource.h"
#include "PCHunter.h"
#include "Utils/ControlBuilder.h"
#include <atlstr.h>
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Authority/Process.h"

#define		TIMER_PCHUNTER_ID			0x1000
#define		WM_TRAYICON					WM_USER+0x1000

CPCHunter::CPCHunter()
	:m_nHeadTrans(0)
	,m_strBkImage(_T("bg.png"))
	,_m(0)
	,m_pLastPage(NULL)
	,m_pCurrentPage(NULL)
{
	// ת�������������ѹ��
	AddVirtualWnd(VIRTUAL_WND_TASK,&m_TaskMgr);
	m_TaskMgr.SetVirtualWnd(this, &m_PaintManager);

	AddVirtualWnd(VIRTUAL_WND_NETWORK, &m_NetworkMgr);
	m_NetworkMgr.SetVirtualWnd(this, &m_PaintManager);

	AddVirtualWnd(VIRTUAL_WND_HARD, &m_HardMgr);
	m_HardMgr.SetVirtualWnd(this, &m_PaintManager);

	AddVirtualWnd(VIRTUAL_WND_SYSTEM, &m_SystemMgr);
	m_SystemMgr.SetVirtualWnd(this, &m_PaintManager);
}

CPCHunter::~CPCHunter()
{
	RemoveVirtualWnd(VIRTUAL_WND_TASK);
	m_TaskMgr.ClearVirtualWnd(this);

	RemoveVirtualWnd(VIRTUAL_WND_NETWORK);
	m_NetworkMgr.ClearVirtualWnd(this);

	RemoveVirtualWnd(VIRTUAL_WND_HARD);
	m_HardMgr.ClearVirtualWnd(this);

	RemoveVirtualWnd(VIRTUAL_WND_SYSTEM);
	m_SystemMgr.ClearVirtualWnd(this);
	//m_Tray.DeleteTrayIcon();
	//	�ͷ��̴߳洢�ռ�
	ReleaseProcessErrorInfo();
}

DUI_BEGIN_MESSAGE_MAP(CPCHunter, CNotifyPump)  
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()  

LRESULT CPCHunter::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_DEVICECHANGE:
			return	m_HardMgr.DeviceChanged(wParam, lParam);
		case WM_CHANGECBCHAIN:
				m_HardMgr.OnChangeCbChain(wParam, lParam);
			return FALSE;
		case WM_DRAWCLIPBOARD:
				m_HardMgr.OnDrawClipboard();
			return FALSE;
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CPCHunter::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT bRes = FALSE;
	bHandled = TRUE;
	switch(uMsg)
	{
		case WM_TRAYICON:
				bRes = OnTrayIcon(wParam,lParam);
			break;
		case WM_MENUCLICK:
				bRes = OnMenuClick(wParam,lParam);
			break;
		default:
			bHandled = FALSE;
	}

	if (bHandled) return bRes;

	return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
}

void CPCHunter::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CPCHunter::GetWindowClassName() const
{
	return _T("PCHunter");
}

LPCTSTR CPCHunter::GetResourceID() const
{
	return _T("");
}

CDuiString CPCHunter::GetZIPFileName() const
{
	return _T("PCHunter.zip");
}

CDuiString CPCHunter::GetSkinFile()
{
	return _T("PCHunter.xml");
}

CDuiString CPCHunter::GetSkinFolder()
{
	return _T("PCHunter");
}

UILIB_RESOURCETYPE CPCHunter::GetResourceType() const
{
	return UILIB_FILE;
}

void CPCHunter::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	CHorizontalLayoutUI* pControl = (CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("TabSwitch"));
	if (pControl)
		m_PaintManager.SendNotify(pControl->GetItemAt(0), DUI_MSGTYPE_SELECTCHANGED);
}

CControlUI* CPCHunter::CreateControl(LPCTSTR pstrClass)
{
	CControlBuilder RootBuilder;
	return RootBuilder.CreateControl(pstrClass);
}

CPaintManagerUI* CPCHunter::GetMainWndPaintManager()
{
	return &m_PaintManager;
}

DWORD CPCHunter::GetBkColor()
{
	CVerticalLayoutUI* pVLayout = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("VHeader"));
	return pVLayout->GetBkColor();
}

void CPCHunter::SetBkColor(DWORD crColor)
{
	CVerticalLayoutUI* pVLayout = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("VHeader"));
	pVLayout->SetBkColor(crColor);
}

CDuiString CPCHunter::GetBkImage()
{
	/*CVerticalLayoutUI* pVLayout = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("VHeader"));
	return pVLayout->GetBkImage();*/
	return m_strBkImage;
}

void CPCHunter::SetBkImage(LPCTSTR lpszBkImage)
{
	m_strBkImage = lpszBkImage;
	SetHeadTrans(lpszBkImage,0);
}

void CPCHunter::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnClose"))){
		Close(IDOK);
		PostQuitMessage(0);
	}	
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnMin")))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("btn")))
	{	
		CChartViewUI* pChartView = (CChartViewUI*)m_PaintManager.FindControl(_T("ChartView"));
		CDuiString mLegend;
		mLegend.Format(_T("����ͼ��%d"),++_m);
		CChartSeries* pSeriesA = new CChartSeries(mLegend,RGB(rand()%256,rand()%256,rand()%256),RGB(rand()%256,rand()%256,rand()%256));

		for(int nIndex = 0;(UINT)nIndex < pChartView->GetGroupCount();nIndex++){
			pSeriesA->AddSeriesData(rand()%50);
		}
		pChartView->AddSeries(pSeriesA);
	}
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnMenu")))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		const RECT& rcPos = msg.pSender->GetPos();
		CDuiPoint pt(rcPos.left,rcPos.bottom);
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("Menu.xml"));
		pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);
	}
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnSkin")))
	{
		CSkinPickerDialog pSkinPicker(_T("SkinPickerDialog.xml"),this);
		pSkinPicker.Create(m_hWnd,_T("Ƥ���봰�ڵ���"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
		pSkinPicker.ShowModal();
	}
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnLogo")))
	{
		CAboutDialog* pDlg = new CAboutDialog(m_hWnd);
		pDlg->ShowModal();
	}
}

void CPCHunter::OnTabSelect(TNotifyUI& msg)
{
	
}

void CPCHunter::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetParent() == (CControlUI*)m_PaintManager.FindControl(_T("TabSwitch")))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("TabWizard"));
		if (pTabLayout == NULL)
			return;

		UINT nIndex = _ttoi(msg.pSender->GetUserData());
		pTabLayout->SelectItem(nIndex);

		CControlUI* pMsgNotify = GetViewObject(pTabLayout,nIndex);
		if (pMsgNotify)
			m_PaintManager.SendNotify(pMsgNotify, DUI_MSGTYPE_LOADITEM);
	}
}

void CPCHunter::OnValueChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("Widget_slider"))
	{
		switch((SLIDER_TYPE)msg.pSender->GetTag())
		{
		case SLIDER_TYPE_CHANGE_WNDTRANS:
				SetHeadTrans(NULL,static_cast<CSliderUI*>(msg.pSender)->GetValue());
			break;
		case SLIDER_TYPE_CHANGE_LISTTRANS:
				SetListTrans(static_cast<CSliderUI*>(msg.pSender)->GetValue());
			break;
		}
	}
}

void CPCHunter::SetHeadTrans(LPCTSTR lpszBkImage /*= NULL*/,int nValue /*= 0*/)
{
	CControlUI* pTabLayout = m_PaintManager.FindControl(_T("VHeader"));

	CString strHeaderImage;
 	CString strBkImage(lpszBkImage);
 	if (strBkImage.IsEmpty())
		strBkImage = m_strBkImage;

	if (nValue != 0)
		m_nHeadTrans = nValue;

	DWORD dwOpacity = 255-m_nHeadTrans;
	strHeaderImage.Format(_T("file='%s' fade='%d'"),strBkImage,dwOpacity);
	pTabLayout->SetBkImage(strHeaderImage);
}

void CPCHunter::SetListTrans(int nValue)
{
	CControlUI* pControl = m_PaintManager.FindControl(_T("TabWizard"));

	CString strBkImage;
	strBkImage.Format(_T("file='white.png' fade='%d'"),255-nValue);
	pControl->SetBkImage(strBkImage);
}

CControlUI* CPCHunter::GetViewObject(CTabLayoutUI* pTabLayout,int nIndex)
{
	CVerticalLayoutUI* pSubObject = (CVerticalLayoutUI*)pTabLayout->GetItemAt(nIndex);
	CTabLayoutUI* pSubTab = (CTabLayoutUI*)pSubObject->FindSubControlByClassName(_T("TabLayoutUI"));
	if (pSubTab == NULL )
		return NULL;

	int nSelectId = pSubTab->GetCurSel();
	return pSubTab->GetItemAt(nSelectId);
}

LRESULT CPCHunter::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	LRESULT lRes = FALSE;
	if (wParam == VK_F5)
		lRes =  OnF5Down(uMsg, wParam, lParam, bHandled);
	else
		lRes =  __super::OnKeyDown(uMsg, wParam, lParam, bHandled);

	return lRes;
}

LRESULT CPCHunter::OnF5Down(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CPCHunter::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	UINT uMsgId=LOWORD(lParam);

	if (uMsgId==WM_RBUTTONUP)
	{
		CMenuWnd* pMenu = new CMenuWnd();
		CDuiPoint pt;
		GetCursorPos(&pt);
		STRINGorID strXmlFile(_T("MenuStatus.xml"));
		pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);
	}
	else if (uMsgId == WM_MOUSEMOVE)
		m_Tray.OnMouseMove(); 
	else if (uMsgId == WM_MOUSEHOVER)
	{
		/*if (m_pMsgTip == NULL)
			m_pMsgTip = new CMessageTip(m_hWnd,_T(""),0xFFFFFFFF,_T("��������"),_T(""));
		
		m_pMsgTip->ShowWindow(true);*/
	}
	else if (uMsgId == WM_MOUSELEAVE)
	{
		/*POINT pt;
		GetCursorPos(&pt);
		RECT rcPos;
		GetWindowRect(m_pMsgTip->GetHWND(),&rcPos);
		if (PtInRect(&rcPos,pt) == TRUE)
		{
			m_Tray.SetTrackMouse(TRUE);
		}
		else if (m_pMsgTip->GetHWND() != NULL && IsWindow(m_pMsgTip->GetHWND()))
		{
			m_pMsgTip->ShowWindow(false);
		}*/
	}

	return TRUE;
}

LRESULT CPCHunter::OnMenuClick(WPARAM wParam, LPARAM lParam)
{
	CControlUI* pControl = (CControlUI*)lParam;
	ASSERT(pControl);
	if (pControl == NULL)
		return FALSE;

	//��ȡ�˵���Ӧ���ڵ�Ŀؼ�����
	CDuiString strMenuName = pControl->GetManager()->GetRoot()->GetName();
	if (strMenuName == _T("AppMenu"))
		m_TaskMgr.OnAppMenu(pControl);
	else if (strMenuName == _T("ScanResultMenu"))
		m_NetworkMgr.OnHostScanMenu(pControl);
	else if (strMenuName == _T("RangeMenu"))
		m_NetworkMgr.OnRangeMenu(pControl);
	else if (strMenuName == _T("RouteInfo"))
		m_NetworkMgr.OnRouteInfo(pControl);
	else if (strMenuName == _T("TrayMenu"))
		m_SystemMgr.OnTrayMenu(pControl);
	return TRUE;
}
