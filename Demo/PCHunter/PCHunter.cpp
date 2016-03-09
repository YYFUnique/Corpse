#include "stdafx.h"
#include "Resource.h"
#include "PCHunter.h"
#include "SkinPickerDialog.h"
#include "Utils/TextTools.h"
#include <atlstr.h>

#define		TIMER_PCHUNTER_ID			0x1000
#define		WM_TRAYICON					WM_USER+0x1000

CPCHunter::CPCHunter()
	:m_nHeadTrans(0)
	,m_strBkImage(_T("bg.png"))
	,_m(0)
	,m_pLastPage(NULL)
	,m_pCurrentPage(NULL)
{
	AddVirtualWnd(_T("task"),&m_TaskMgr);
	AddVirtualWnd(_T("network"),&m_NetMgr);

	m_pMsgTip = NULL;
}

CPCHunter::~CPCHunter()
{
	RemoveVirtualWnd(_T("task"));
	RemoveVirtualWnd(_T("network"));

	if (m_pDropTarget)
		m_pDropTarget->DragDropRevoke(m_hWnd);

	//m_Tray.DeleteTrayIcon();
}

LRESULT CPCHunter::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

CControlUI* CPCHunter::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pControl = NULL;

	CDuiString strControlName;
	strControlName.Format(_T("%s 没有定义！请确认"),pstrClass);
	ASSERT(FALSE && _T("控件没有定义"));

	return pControl;
}

void CPCHunter::Notify(TNotifyUI& msg)
{
	if (msg.sVirtualWnd.IsEmpty() == false)
	{
		CVirtualWnd* pObject = FindObjectByName(msg.sVirtualWnd);
		if (pObject != NULL)
			pObject->Notify(msg);
		else 
			__super::Notify(msg);
	}
	else if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_VALUECHANGED)
		OnValueChanged(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);
}

CVirtualWnd* CPCHunter::FindObjectByName(LPCTSTR lpszVirtualWndName)
{
	for (int n=0;n<m_SubNotifys.GetSize();++n)
	{
		CVirtualWnd* pObject = (CVirtualWnd*)m_SubNotifys.GetAt(n);
		if (pObject && StrStrI(pObject->GetVirtualWnd(),lpszVirtualWndName) != NULL)
			return pObject;
	}

	return NULL;
}

void CPCHunter::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	//m_Tray.CreateTrayIcon(m_hWnd,IDI_MAINFRAME,_T("系统信息查看工具 V1.0"),WM_TRAYICON);
	m_pDropTarget = new CDropTargetEx;
	m_pDropTarget->DragDropRegister(&m_PaintManager,m_hWnd);

	CVirtualWnd::SetPaintMagager(&m_PaintManager);

	m_SubNotifys.Add(&m_TaskMgr);
	m_SubNotifys.Add(&m_NetMgr);

	CHorizontalLayoutUI* pControl = (CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("TabSwitch"));
	m_PaintManager.SendNotify(pControl->GetItemAt(0), DUI_MSGTYPE_SELECTCHANGED);
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
		////m_PaintManager.SetShadow(false);
		//m_PaintManager.SetLayered(false);
		////AnimateWindow(m_hWnd,1000,AW_HIDE|AW_BLEND); 
		PostQuitMessage(0);
	}	
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnMin")))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("btn")))
	{	
		CChartViewUI* pChartView = (CChartViewUI*)m_PaintManager.FindControl(_T("ChartView"));
		CDuiString mLegend;
		mLegend.Format(_T("测试图例%d"),++_m);
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
		CPoint pt(rcPos.left,rcPos.bottom);
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("Menu.xml"));
		pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);
	}
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnSkin")))
	{
		CSkinPickerDialog pSkinPicker(_T("SkinPickerDialog.xml"),this);
		pSkinPicker.Create(m_hWnd,_T("皮肤与窗口调整"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
		pSkinPicker.ShowModal();
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
			m_PaintManager.SendNotify(pMsgNotify,DUI_MSGTYPE_SETFOCUS);
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
		CPoint pt;
		GetCursorPos(&pt);
		STRINGorID strXmlFile(_T("MenuStatus.xml"));
		pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);
	}
	else if (uMsgId == WM_MOUSEMOVE)
		m_Tray.OnMouseMove(); 
	else if (uMsgId == WM_MOUSEHOVER)
	{
		if (m_pMsgTip == NULL)
			m_pMsgTip = new CMessageTip(m_hWnd,_T(""),0xFFFFFFFF,_T("我欲飞翔"),_T(""));
		
		m_pMsgTip->ShowWindow(true);
	}
	else if (uMsgId == WM_MOUSELEAVE)
	{
		POINT pt;
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
		}
	}

	return TRUE;
}

LRESULT CPCHunter::OnMenuClick(WPARAM wParam, LPARAM lParam)
{
	CControlUI* pControl = (CControlUI*)lParam;
	ASSERT(pControl);
	if (pControl == NULL)
		return FALSE;

	CDuiString strName = pControl->GetManager()->GetRoot()->GetName();
	//主机扫描菜单响应
	if (strName == _T("ScanResultMenu"))
		OnHostScanMenu(pControl);
	else if (strName == _T("RangeMenu"))
		OnRangeMenu(pControl);
	return TRUE;
}

void CPCHunter::OnHostScanMenu(CControlUI* pControl)
{
	CListUI* pList = (CListUI*)m_PaintManager.FindControl(_T("Scan"));
	if (pList->GetCurSel() == -1)
		return;

	CDuiString strName = pControl->GetName();
	CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(pList->GetCurSel());
		//pList->GetCurSel();
	if (strName == _T("CopyIPAddress"))
		CopyDataToClipboard(CF_UNICODETEXT, m_hWnd, pItem->GetText(0));
	else if (strName == _T("CopyMacAddress"))
		CopyDataToClipboard(CF_UNICODETEXT, m_hWnd, pItem->GetText(1));
	else if (strName == _T("CopyUserName"))
		CopyDataToClipboard(CF_UNICODETEXT, m_hWnd, pItem->GetText(5));
}

void CPCHunter::OnRangeMenu(CControlUI* pControl)
{
	CDuiString strMenuText = pControl->GetName();
	if (strMenuText.IsEmpty())
		return;

	CEditUI2* pStartIP = (CEditUI2*)m_PaintManager.FindControl(_T("EditStartIP"));
	CEditUI2* pEndIP = (CEditUI2*)m_PaintManager.FindControl(_T("EditEndIP"));

	TCHAR szNetIP[20], szBroadcastIP[20];
	_stscanf(strMenuText, _T("%[^|]|%s"), szNetIP, szBroadcastIP);

	pStartIP->SetText(szNetIP);
	pEndIP->SetText(szBroadcastIP);
}