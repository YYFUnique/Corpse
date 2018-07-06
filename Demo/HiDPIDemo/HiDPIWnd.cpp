#include "StdAfx.h"
#include "HiDPIWnd.h"
#include "resource.h"

CHiDPIWnd::CHiDPIWnd()
{

}

CHiDPIWnd::~CHiDPIWnd()
{

}

void CHiDPIWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CHiDPIWnd::GetWindowClassName() const
{
	return _T("HiDPIWnd Demo");
}

UILIB_RESOURCETYPE CHiDPIWnd::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CHiDPIWnd::GetSkinFile()
{
	return _T("Main_dlg.xml");
}

CDuiString CHiDPIWnd::GetSkinFolder()
{
	return _T("HiDPITest");
}

void CHiDPIWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);
}

void CHiDPIWnd::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

void CHiDPIWnd::OnClick(TNotifyUI& msg)
{
	CDuiString strNotifyUIName = msg.pSender->GetName();

	if (strNotifyUIName == _T("BtnClose"))
		PostQuitMessage(0);
	else if (strNotifyUIName == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (strNotifyUIName == _T("BtnMenu"))
	{
		//RECT rcPos = msg.pSender->GetPos();
		
		POINT pt;// = {rcPos.right,rcPos.bottom};
		GetCursorPos(&pt);
		//ClientToScreen(m_hWnd,&pt);
		//CMenuWnd* pMenu = CMenuWnd::CreateMenu(NULL, _T("menutest.xml"), point, &m_pm, NULL, eMenuAlignment_Bottom);
		CMenuWnd* pMenu = new CMenuWnd;
		pMenu->Init(NULL, _T("menutest.xml"),pt,&m_PaintManager,NULL/*,eMenuAlignment_Right|eMenuAlignment_Top*/);

		// 先获取到根项，然后就可以使用rootMenu插到到菜单内的任意子菜单项，然后做添加删除操作
		CMenuUI* rootMenu = pMenu->GetMenuUI();
		if (rootMenu != NULL)
		{
			CMenuElementUI* pNew = new CMenuElementUI;
			pNew->SetName(_T("Menu_Dynamic"));
			pNew->SetText(_T("动态一级菜单"));
			pNew->SetShowExplandIcon(true);
			pNew->SetIcon(_T("WebSit.png"));
			pNew->SetIconSize(16,16);

			CMenuElementUI* pSubNew = new CMenuElementUI;
			pSubNew->SetText(_T("动态二级菜单"));
			pSubNew->SetName(_T("Menu_Dynamic"));
			pSubNew->SetIcon(_T("Virus.png"));
			pSubNew->SetIconSize(16,16);
			pNew->Add(pSubNew);

			rootMenu->Add(pNew);

			CMenuElementUI* pNew2 = new CMenuElementUI;
			pNew2->SetName(_T("Menu_Dynamic"));
			pNew2->SetText(_T("动态一级菜单2"));
			rootMenu->AddAt(pNew2,2);
		}

		// 动态添加后重新设置菜单的大小
		pMenu->ResizeMenu();
	}
	else if (strNotifyUIName == _T("DPI96"))
		SetDPI(96);
	else if (strNotifyUIName == _T("DPI120"))
		SetDPI(120);
}

void CHiDPIWnd::OnSelectChanged(TNotifyUI& msg)
{
	CDuiString strNotifyUIName = msg.pSender->GetName();

	if (strNotifyUIName == _T("DPI96"))
		SetDPI(96);
	else if (strNotifyUIName == _T("DPI120"))
		SetDPI(120);
	else if (strNotifyUIName == _T("DPI144"))
		SetDPI(144);
	else if (strNotifyUIName == _T("DPI168"))
		SetDPI(168);
	else if (strNotifyUIName == _T("DPI192"))
		SetDPI(192);
}

void CHiDPIWnd::SetDPI(int iDPI)
{
	m_PaintManager.SetDPI(iDPI);
}