#include "../stdafx.h"
#include "MessageTip.h"

CMessageTip::CMessageTip(LPCTSTR lpszBkFile,DWORD dwBkColor,LPCTSTR lpszWinTitle,LPCTSTR lpszMessage)
:m_strBkFile(lpszBkFile)
,m_dwBkColor(dwBkColor)
,m_strWinTitle(lpszWinTitle)
,m_strMessage(lpszMessage)
{
	Create(NULL,_T("MessageTip"),WS_POPUP|UI_WNDSTYLE_DIALOG,  WS_EX_TOOLWINDOW, 0, 0);
}

CMessageTip::~CMessageTip()
{

}

LPCTSTR CMessageTip::GetWindowClassName() const
{
	return _T("LsGuiFoundation");
}

void CMessageTip::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CMessageTip::OnFinalMessage(HWND hWnd)
{
	RemoveObserver();
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CMessageTip::InitWindow()
{
	int iScreenX = GetSystemMetrics(SM_CXSCREEN);
	int iScreenY = GetSystemMetrics(SM_CYSCREEN);

	//得到任务栏句柄
	HWND hWnd =::FindWindow(_T("Shell_TrayWnd"),NULL); 
	RECT rcTray;
	GetWindowRect(hWnd,&rcTray);

	SIZE size = m_PaintManager.GetInitSize();
	SetWindowPos(m_hWnd,HWND_TOPMOST,rcTray.right, iScreenY - size.cy, size.cx, size.cy, SWP_SHOWWINDOW);
	CLabelUI* pLabelTitle = (CLabelUI*)m_PaintManager.FindControl(_T("WinTitle"));
	pLabelTitle->SetText(m_strWinTitle);

	CTextUI* pTextMessage = (CTextUI*)m_PaintManager.FindControl(_T("DialogTipInfo"));
	pTextMessage->SetText(m_strMessage);
}

CDuiString CMessageTip::GetSkinFile()
{
	return _T("MessageTip.xml");
}

CDuiString CMessageTip::GetSkinFolder()
{
	return _T("pcmaster");
}

BOOL CMessageTip::Receive(SkinChangedParam lParam)
{
	m_strBkFile = lParam.bgimage;
	m_dwBkColor = lParam.bkcolor;
	CControlUI* pBackground = m_PaintManager.FindControl(_T("VLayoutTotal"));
	if (pBackground != NULL)
	{
		if (m_strBkFile.IsEmpty() == FALSE)
		{
			CDuiString strBkImage;
			strBkImage.Format(_T("file='%s' corner='600,200,1,1'"),m_strBkFile);
			pBackground->SetBkImage(strBkImage);
		}
		else
			pBackground->SetBkImage(_T(""));

		pBackground->SetBkColor(m_dwBkColor);
	}

	return TRUE;
}

LRESULT CMessageTip::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

void CMessageTip::OnClick(TNotifyUI& msg)
{
	Close();
}