#include "StdAfx.h"
#include "FileDiskMain.h"
#include "resource.h"
#include "Menu.h"
 
 CFileDiskMain::CFileDiskMain()
 {
 	
 }
 
 CFileDiskMain::~CFileDiskMain()
 {
 
 }
 
 DUI_BEGIN_MESSAGE_MAP(CFileDiskMain,WindowImplBase)
 	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
 DUI_END_MESSAGE_MAP()
 
 LPCTSTR CFileDiskMain::GetWindowClassName() const
 {
 	return _T("DuiLib::FileDisk");
 }
 
 CDuiString CFileDiskMain::GetSkinFile()
 {
 	return _T("VirtualDisk.xml");
 }
 
 CDuiString CFileDiskMain::GetSkinFolder()
 {
 	return _T("skin");
 }

 UILIB_RESOURCETYPE CFileDiskMain::GetResourceType() const
 {
	 return UILIB_FILE;
 }
 
 void CFileDiskMain::Notify(TNotifyUI& msg)
 {
 	CDuiString strCtlName = msg.pSender->GetName();
 
 	if (strCtlName == _T("CloseBtn"))
 		Close();
 	else if (strCtlName == _T("MinBtn"))
 		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
 	else if (strCtlName == _T("MenuBtn"))
 	{
 		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
 		CPoint point = msg.ptMouse;
 		ClientToScreen(m_hWnd, &point);
 		pMenu->Init(NULL, _T("menu.xml"), point);
 	}
 
 	if (msg.sType == _T("setfocus"))
 	{
 		CTabLayoutUI* pControl = (CTabLayoutUI*)m_PaintManager.FindControl(_T("DiskTabLayout"));
 		if (strCtlName == _T("LoadDisk"))
 			pControl->SelectItem(0);
 		else if (strCtlName == _T("NewDisk"))
 			pControl->SelectItem(1);
 		else if (strCtlName == _T("UnloadDisk"))
 			pControl->SelectItem(2);
 	}
 }
 
 CControlUI* CFileDiskMain::CreateControl(LPCTSTR pstrClass)
 {
 	return NULL;
 }
 
 void CFileDiskMain::InitWindow()
 {
 	SetIcon(IDI_SMALL);
 	//m_Shadow.Create(m_hWnd);
 }
 
 void CFileDiskMain::OnClick(DuiLib::TNotifyUI &msg)
 {
 
 }
 
 LRESULT CFileDiskMain::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
 {
 	//m_Shadow.Create(m_hWnd);
 	return WindowImplBase::OnCreate(uMsg,wParam,lParam,bHandled);
 }
 
 LRESULT CFileDiskMain::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
 {
 	switch(uMsg)
 	{
 		case WM_MENUCLICK:
 				OnMenuItemClick((CControlUI*)lParam);
 			break;
 		default:
 			bHandled = FALSE;
 	}
 	return TRUE;
 }
 
 void CFileDiskMain::OnMenuItemClick(CControlUI* pControl)
 {
 	ASSERT(pControl);
 	MessageBox(m_hWnd,pControl->GetName(),_T("≤Àµ•Ã· æ"),MB_OK);
 }

