#include "StdAfx.h"
#include "TxPCMain.h"
#include "Resource.h"
#include "UIAnimButton.h"
#include "UINavigateBar.h"
#include "UINavigateNode.h"

#define TIMER_ID_SWITCH_PIONEER	(0x2000)
#define TIMER_ID_SWITCH_CLASSIC		(0x2001)

int CTxPCMain::m_nMoveNumber = 0;

CTxPCMain::CTxPCMain()
{

}

CTxPCMain::~CTxPCMain()
{

}

void CTxPCMain::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CTxPCMain::GetWindowClassName() const
{
	return _T("TxPCMain");
}

UILIB_RESOURCETYPE CTxPCMain::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CTxPCMain::GetSkinFile()
{
	return _T("TxPCMgr.xml");
}

CDuiString CTxPCMain::GetSkinFolder()
{
	return _T("TxPCMgr\\xml");
}

void CTxPCMain::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);
}

void CTxPCMain::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

void CTxPCMain::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
	{
		Close(IDOK);
		PostQuitMessage(0);
	}
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
}

void CTxPCMain::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnSwitch")))
	{
		COptionUI* pBtnSwitch = (COptionUI*)msg.pSender;
		BOOL bSelected = pBtnSwitch->IsSelected();
		
		if (bSelected)
			SetTimer(m_hWnd, TIMER_ID_SWITCH_PIONEER, 10, (TIMERPROC)&CTxPCMain::TimerProc);
		else
			SetTimer(m_hWnd, TIMER_ID_SWITCH_CLASSIC, 10, (TIMERPROC)&CTxPCMain::TimerProc);
	}
}

void CALLBACK CTxPCMain::TimerProc(HWND hWnd, UINT uMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
	RECT rcWnd;
	GetWindowRect(hWnd, &rcWnd);
	if (nIDEvent == TIMER_ID_SWITCH_PIONEER)	// 常规模式（尺寸比较大）
	{
		if (rcWnd.right - rcWnd.left > 930)
		{
			// 调整部分控件属性
			//	调整HLayoutFoot子控件用以显示名称
			m_nMoveNumber = 0;
			KillTimer(hWnd, nIDEvent);
		} else {
			if (m_nMoveNumber < 11)
			{
				SetWindowPos(hWnd, HWND_NOTOPMOST, rcWnd.left - 25, rcWnd.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW | SWP_NOSENDCHANGING);
			}
			else
			{
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, rcWnd.right - rcWnd.left + 30, rcWnd.bottom - rcWnd.top, SWP_NOMOVE|SWP_SHOWWINDOW | SWP_NOSENDCHANGING);
			}

			m_nMoveNumber++;
		}
	}
	else if (nIDEvent == TIMER_ID_SWITCH_CLASSIC)	// 经典模式（尺寸比较小）
	{
		if (rcWnd.right - rcWnd.left < 394)
		{
			// 调整部分控件属性
			if (m_nMoveNumber == 0)
			{
				// 调整HLayoutFoot子控件，用于隐藏控件名称
			}

			++m_nMoveNumber;
			SetWindowPos(hWnd, HWND_NOTOPMOST, rcWnd.left + 25, rcWnd.top, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW | SWP_NOSENDCHANGING);
			if (m_nMoveNumber >= 11)
			{
				m_nMoveNumber = 0;
				KillTimer(hWnd, nIDEvent);
			}
		} else {
			// 调整部分控件属性
			SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, rcWnd.right - rcWnd.left - 30, rcWnd.bottom - rcWnd.top, SWP_NOMOVE|SWP_SHOWWINDOW | SWP_NOSENDCHANGING);
		}
	}
}

CControlUI* CTxPCMain::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pControl = NULL;
	if (_tcsicmp(pstrClass, _T("animbutton")) == 0)
		pControl = new CAnimButtonUI;
	else if (_tcsicmp(pstrClass, _T("navigatenode")) == 0)
		pControl = new CNavigateNodeUI;
	else if (_tcsicmp(pstrClass, _T("navigatebar")) == 0)
		pControl = new CNavigateBarUI;
	return pControl;
}