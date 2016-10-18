#include "StdAfx.h"
#include "Login.h"
#include "resource.h"
#include "MutileDesktop.h"

CLogin::CLogin()
{

}

CLogin::~CLogin()
{

}

void CLogin::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CLogin::GetWindowClassName() const
{
	return _T("DuilibLogin");
}

UILIB_RESOURCETYPE CLogin::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CLogin::GetZIPFileName() const
{
	return _T("login.xml");
	return _T("");
}

CDuiString CLogin::GetSkinFile()
{
	return _T("login.xml");
}

CDuiString CLogin::GetSkinFolder()
{
	return _T("MutileDesktop");
}

void CLogin::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CLogin::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

void CLogin::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
		PostQuitMessage(0);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnLogin")))
		OnLogin();
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CLogin::OnLogin()
{
	//在线程中创建2个窗口，分别用来切换虚拟桌面
	CMutileDesktop::CreateDesktop(DESKTOP_TYPE_SECURITY);
	CMutileDesktop::CreateDesktop(DESKTOP_TYPE_SYSTEM);

	//切换到虚拟桌面去
	CMutileDesktop::SwitchDesktop(DESKTOP_TYPE_SYSTEM);

	Close(IDOK);
}

