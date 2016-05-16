#include "stdAfx.h"
#include "AddUserName.h"

CAddUserName::CAddUserName(HWND hParent)
:m_hParent(hParent)
{
	Create(m_hParent, _T("AddUserName"), WS_POPUP|UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0);
}

CAddUserName::~CAddUserName()
{
	delete this;
}

void CAddUserName::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

LPCTSTR CAddUserName::GetWindowClassName() const
{
	return _T("UserNameWnd");
}

UILIB_RESOURCETYPE CAddUserName::GetResourceType() const
{
	return UILIB_FILE;
}

LPCTSTR CAddUserName::GetResourceID() const
{
	return _T("");
}

CDuiString CAddUserName::GetZIPFileName() const
{
	return _T("HostScan.zip");
}

CDuiString CAddUserName::GetSkinFile()
{
	return _T("UserName.xml");
}

CDuiString CAddUserName::GetSkinFolder()
{
	return _T("HostScan");
}

void CAddUserName::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CAddUserName::InitWindow()
{
	CenterWindow();
	/*CControlUI* pEditControl = m_PaintManager.FindControl(_T("EditUserName"));
	pEditControl->SetFocus();*/
}

LRESULT CAddUserName::ResponseDefaultKeyEvent(WPARAM wParam)
{
	BOOL bSuccess = FALSE;
	if (wParam == VK_ESCAPE)
	{
		Close();
		bSuccess = TRUE;
	}

	return bSuccess;
}

LPCTSTR CAddUserName::GetUserName()
{
	return m_strUserName;
}

void CAddUserName::SetUserName(LPCTSTR lpszUserName)
{
	CEditUI2* pUserName = (CEditUI2*)m_PaintManager.FindControl(_T("EditUserName"));
	if (pUserName)
		pUserName->SetText(lpszUserName);
}

void CAddUserName::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
		Close(IDCANCEL);
	else if (msg.pSender->GetName() == _T("BtnCloseBox"))
		Close(IDCANCEL);
	else if (msg.pSender->GetName() == _T("BtnAddUserName"))
	{
		SetUserNameByEdit();
		Close(IDOK);
	}
}

void CAddUserName::SetUserNameByEdit()
{
	CControlUI* pEditControl = m_PaintManager.FindControl(_T("EditUserName"));
	m_strUserName = pEditControl->GetText();
}
