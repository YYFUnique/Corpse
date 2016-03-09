#include "../stdafx.h"
#include "ColorSkinWindow.h"

CColorSkinWindow::CColorSkinWindow(CPCMaster* pPCMaster, CDuiRect pRect)
:m_pPCMaster(pPCMaster)
,m_lpRectParent(pRect)
{
	Create(NULL, _T("ColorSkin"), WS_POPUP|UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0);
	ShowWindow();
}

CColorSkinWindow::~CColorSkinWindow()
{

}

LPCTSTR CColorSkinWindow::GetWindowClassName() const
{
	return _T("DuilibColorSkinWindow");
}

void CColorSkinWindow::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if(msg.sType == DUI_MSGTYPE_VALUECHANGED)
		OnValueChanged(msg);
	else if(msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);
}

void CColorSkinWindow::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CColorSkinWindow::InitWindow()
{
	SIZE size = m_PaintManager.GetInitSize();
	SetWindowPos(m_hWnd,HWND_TOPMOST,m_lpRectParent.right - size.cx, m_lpRectParent.top, size.cx, size.cy, SWP_SHOWWINDOW);
}

CDuiString CColorSkinWindow::GetSkinFile()
{
	return _T("ColorWnd.xml");
}

CDuiString CColorSkinWindow::GetSkinFolder()
{
	return _T("pcmaster");
}

LRESULT CColorSkinWindow::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	return TRUE;
}

LRESULT CColorSkinWindow::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

void CColorSkinWindow::OnClick(TNotifyUI& msg)
{
	CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLayout")));
	if (pTabControl != NULL)
	{
		if (pTabControl->GetCurSel() == 0 && _tcsstr(msg.pSender->GetName(),_T("BtnColorUI")) != 0) //如果点击的是切换背景颜色
		{
			CSliderUI* pAdjustColorSliderR = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("AdjustColorSliderR")));
			CSliderUI* pAdjustColorSliderG = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("AdjustColorSliderG")));
			CSliderUI* pAdjustColorSliderB = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("AdjustColorSliderB")));
			if ((pAdjustColorSliderR != NULL) && (pAdjustColorSliderG != NULL) && (pAdjustColorSliderB != NULL))
			{
				DWORD dwColor = msg.pSender->GetBkColor();
				pAdjustColorSliderR->SetValue(static_cast<BYTE>(GetRValue(dwColor)));
				pAdjustColorSliderG->SetValue(static_cast<BYTE>(GetGValue(dwColor)));
				pAdjustColorSliderB->SetValue(static_cast<BYTE>(GetBValue(dwColor)));

				m_pPCMaster->SetBkColor(dwColor);
			}
		}
		else if(pTabControl->GetCurSel() == 1 && _tcsstr(msg.pSender->GetName(),_T("BtnSkinUI")) != 0)
		{
			//MessageBox(m_hWnd,_T("该功能还未开放，请留意最新动态^_^！"),_T("提示"),MB_OK|MB_ICONINFORMATION);
		}
	}
}

void CColorSkinWindow::OnValueChanged(TNotifyUI& msg)
{
	CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLayout")));
	if (pTabControl != NULL)
	{
		if (pTabControl->GetCurSel() == 0)
		{
			CSliderUI* pAdjustColorSliderR = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("AdjustColorSliderR")));
			CSliderUI* pAdjustColorSliderG = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("AdjustColorSliderG")));
			CSliderUI* pAdjustColorSliderB = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("AdjustColorSliderB")));
			if ((pAdjustColorSliderR != NULL) && (pAdjustColorSliderG != NULL) && (pAdjustColorSliderB != NULL))
			{
				if (_tcsstr(msg.pSender->GetName(), _T("AdjustColorSlider")) != 0)
				{
					BYTE bRed = pAdjustColorSliderR->GetValue();
					BYTE bGreen = pAdjustColorSliderG->GetValue();
					BYTE bBlue = pAdjustColorSliderB->GetValue();
					COLORREF ColorRef = RGB(bRed, bGreen, bBlue);
					DWORD dwColor = 0xFF000000 | ColorRef;
					m_pPCMaster->SetBkColor(dwColor);
				}
			}
		}
	}
}

void CColorSkinWindow::OnSelectChanged(TNotifyUI& msg)
{
	CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLayout")));
	if (pTabControl != NULL)
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("BtnAdjColor")) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 0)
			{
				pTabControl->SelectItem(0);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("BtnAdjBk")) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 1)
			{
				pTabControl->SelectItem(1);
			}
		}
	}
}