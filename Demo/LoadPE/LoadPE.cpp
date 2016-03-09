#include "stdafx.h"
#include "LoadPE.h"
#include "Resource.h"

CLoadPE::CLoadPE()
{
	
}

LPCTSTR CLoadPE::GetWindowClassName() const
{
	return _T("LoadPE");
}

CDuiString CLoadPE::GetSkinFile()
{
	return _T("main.xml");
}

CDuiString CLoadPE::GetZIPFileName() const
{
	return _T("LoadPE");
}

CDuiString CLoadPE::GetSkinFolder()
{
	return _T("LoadPE");
}

UILIB_RESOURCETYPE CLoadPE::GetResourceType() const
{
	return UILIB_FILE;
}

void CLoadPE::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CLoadPE::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

CControlUI* CLoadPE::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CLoadPE::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("CloseBtn"))
		PostQuitMessage(0);
	else if (msg.pSender->GetName() == _T("MinBtn"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}
