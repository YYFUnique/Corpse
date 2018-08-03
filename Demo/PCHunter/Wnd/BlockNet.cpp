#include "StdAfx.h"
#include "BlockNet.h"

CBlockNet::CBlockNet(HWND hWnd)
{
	Create(hWnd,_T("×è¶ÏÍøÂç"), WS_POPUP|UI_WNDSTYLE_DIALOG,WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CBlockNet::~CBlockNet()
{

}

void CBlockNet::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

CDuiString CBlockNet::GetSkinFile()
{
	return _T("BlockNet.xml");
}

CDuiString CBlockNet::GetSkinFolder()
{
	return _T("PCHunter");
}

void CBlockNet::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CBlockNet::InitWindow()
{
	CenterWindow();
}

LPCTSTR CBlockNet::GetWindowClassName(void) const
{
	return _T("BlockNet");
}

LRESULT CBlockNet::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

void CBlockNet::OnClick(TNotifyUI& msg)
{
	CDuiString strSender = msg.pSender->GetName();
	if (strSender == _T("BtnClose"))
		Close(IDOK);
}

void CBlockNet::SetAttackTargetIP(LPCTSTR lpszTargetIP)
{
	m_strAttackTargetIP = lpszTargetIP;
}

void CBlockNet::SetAttackTargetMAC(LPCTSTR lpszTargetMAC)
{
	m_strAttackTargetMAC = lpszTargetMAC;
}