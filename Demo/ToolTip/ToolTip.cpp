#include "StdAfx.h"
#include "ToolTip.h"
#include "Resource.h"
#include "DllCore/Utils/OsInfo.h"

CToolTip::CToolTip()
{
	m_pToolTip = NULL;
	m_pListView = NULL;
}

CToolTip::~CToolTip()
{
	if (m_pToolTip != NULL)
		m_pToolTip->Close(IDOK);
	if (m_pListView != NULL)
		m_pListView->Close(IDOK);
}

void CToolTip::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CToolTip::GetWindowClassName() const
{
	return _T("ToolTip");
}

UILIB_RESOURCETYPE CToolTip::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIPRESOURCE;
#endif
}

LPCTSTR CToolTip::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_SKIN);
}

CDuiString CToolTip::GetZIPFileName() const
{
	return _T("ToolTip.zip");
}

CDuiString CToolTip::GetSkinFile()
{
	return _T("ToolTip.xml");
}

CDuiString CToolTip::GetSkinFolder()
{
	return _T("ToolTip");
}

void CToolTip::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED)
		OnTextChanged(msg);
	else if (msg.sType == DUI_MSGTYPE_RETURN)
		OnReturn(msg);
	else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnWindowInit(msg);
}

void CToolTip::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	HWND hDesktop = GetDesktopHWND();
	GetDesktopList(hDesktop, m_DesktopInfoList);

	m_pItemName = (CEditUI2*)m_PaintManager.FindControl(_T("ItemName"));
}

LRESULT CToolTip::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandle = FALSE;
	if (uMsg == WM_KEYDOWN) {
		if (m_pListView->OnKeyDown(uMsg, wParam, lParam, bHandle) == FALSE)
			return FALSE;
	} else if (uMsg == WM_WINDOWPOSCHANGING) {
		if (m_pListView == NULL)
			return TRUE;
		return m_pListView->OnPosChanged(uMsg, wParam, lParam, bHandle);
	} else if (uMsg == WM_MOUSEWHEEL) {
		return m_pListView->OnMouseWheel(uMsg, wParam, lParam, bHandle);
	} else if (uMsg == WM_NCLBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN) {
		// 点击应用程序标题栏应该关闭提示框
		if (wParam == HTCAPTION)
			m_pListView->ShowWindow(false);
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CToolTip::OnClose(UINT , WPARAM , LPARAM , BOOL& bHandled)
{
	bHandled = TRUE;
	PostQuitMessage(0);
	return FALSE;
}

void CToolTip::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
		Close(IDOK);
	else if (msg.pSender->GetName() == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (msg.pSender->GetName() == _T("BtnPopup"))
		OnPopup(msg);
}

void CToolTip::OnReturn(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ItemName"))
	{
		m_pListView->ShowWindow(false);
	}
}

void CToolTip::OnTextChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ItemName"))
	{
		CDuiString strItemName = msg.pSender->GetText();

		m_pListView->RemoveAll();

		POSITION pos = m_DesktopInfoList.GetHeadPosition();
		while(pos)
		{
			const DESKTOP_INFO& DesktopInfo = m_DesktopInfoList.GetNext(pos);
			if (strItemName.IsEmpty() || StrStrI(DesktopInfo.strIconName, strItemName) == NULL)
				continue;
			m_pListView->Add(DesktopInfo);
		}
		m_pListView->ResizeWnd();
	}
}

void CToolTip::OnWindowInit(TNotifyUI& msg)
{
	m_pToolTip = new CPopupToolTip;
	m_pListView = new CListViewWnd(m_hWnd, m_pItemName);

	m_pItemName->SetFocus();
}

BOOL CToolTip::GetDesktopList(HWND hDesktop, CDesktopInfoList& DesktopInfoList)
{
	BOOL bRet = FALSE;

	if(Is64BitOS())
		bRet = GetDestopIcon64(hDesktop, DesktopInfoList);
	else
		bRet = GetDestopIcon(hDesktop, DesktopInfoList);
	return bRet;
}

BOOL CToolTip::GetDesktopIcon(HWND hDeskWnd, LPCTSTR lpszItemName, LPRECT lpRect, BOOL bVague/* = FALSE*/)
{
	BOOL bRet = FALSE;

	if(Is64BitOS())
		bRet = GetDesktopIconRect64(hDeskWnd, lpszItemName, lpRect, bVague);
	else
		bRet = GetDesktopIconRect(hDeskWnd, lpszItemName, lpRect, bVague);
	return bRet;
}

void CToolTip::OnPopup(TNotifyUI& msg)
{
	RECT rcPos;
	CEditUI2* pItemName = (CEditUI2*)m_PaintManager.FindControl(_T("ItemName"));
	if (pItemName->GetText().IsEmpty())
	{
		MessageBox(m_hWnd, _T("要查找的名称不能为空！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	HWND hDesktop = GetDesktopHWND();
	// 精确查找失败，通过模糊查找
	if (GetDesktopIcon(hDesktop, pItemName->GetText(), &rcPos) == FALSE)
	{
		BOOL bRet = GetDesktopIcon(hDesktop, pItemName->GetText(), &rcPos, TRUE);
		if (bRet == FALSE)
		{
			MessageBox(m_hWnd, _T("要查找的名称不存在，请重新输入！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
	CEditUI2* pTip = (CEditUI2*)m_PaintManager.FindControl(_T("ToolTip"));
	if (pTip != NULL)
		m_pToolTip->SetTooltip(pTip->GetText());
	SetWindowPos(m_pToolTip->GetHWND(), HWND_TOPMOST, (rcPos.right + rcPos.left ) / 2, rcPos.bottom - 30, 0, 0, SWP_NOSIZE);
	m_pToolTip->Show(TRUE);
}