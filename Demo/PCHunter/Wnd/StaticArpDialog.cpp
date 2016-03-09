#include "stdafx.h"
#include "StaticArpDialog.h"

CStaticArpDialog::CStaticArpDialog(HWND hWnd)
{
	Create(hWnd,_T("Ìí¼Ó¾²Ì¬ARPµØÖ·"), WS_POPUP|UI_WNDSTYLE_DIALOG,WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CStaticArpDialog::~CStaticArpDialog()
{

}

void CStaticArpDialog::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CStaticArpDialog::GetWindowClassName() const
{
	return _T("StaticArpDialog");
}

CDuiString CStaticArpDialog::GetSkinFile()
{
	return _T("StaticArp.xml");
}

CDuiString CStaticArpDialog::GetSkinFolder()
{
	return _T("PCHunter");
}

void CStaticArpDialog::InitWindow()
{
	SIZE size = m_PaintManager.GetInitSize();
	CenterWindow();
	//SetWindowPos(m_hWnd,nu, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
}

LRESULT CStaticArpDialog::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

void CStaticArpDialog::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CStaticArpDialog::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
		Close();
	else if (msg.pSender->GetName() == _T("BtnOk"))
		OnOK(msg);
}

void CStaticArpDialog::OnOK(TNotifyUI& msg)
{

}
