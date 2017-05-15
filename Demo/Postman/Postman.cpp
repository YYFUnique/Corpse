#include "StdAfx.h"
#include "Postman.h"
#include "Resource.h"

#define WM_TEST_MESSAGE		(WM_USER+0x1000)

CPostman::CPostman()
:m_bShowBar(TRUE)
{

}

CPostman::~CPostman()
{

}

void CPostman::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CPostman::GetWindowClassName() const
{
	return _T("Postman");
}

UILIB_RESOURCETYPE CPostman::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CPostman::GetSkinFile()
{
	return _T("Postman.xml");
}

CDuiString CPostman::GetSkinFolder()
{
	return _T("Postman");
}

void CPostman::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_WINDOWSIZE)
		OnSize(msg);
}

void CPostman::InitWindow()
{
	SetIcon(IDI_MAINFRAME);


	for (int n=0;n<10000;++n)
	{
		BOOL bRet = PostMessage(WM_TEST_MESSAGE,0,0);
		if (bRet == FALSE)
		{
			CDuiString strTipInfo;
			strTipInfo.Format(_T("当前检测到错误:%d[%d]."),n+1,GetLastError());
			MessageBox(m_hWnd,strTipInfo,_T("提示"),MB_OK);
		}
	}
}

void CPostman::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
		PostQuitMessage(0);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMax")))
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, NULL);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnRestore")))
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, NULL);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnHide")))
		ShowBar(FALSE);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnShow")))
		ShowBar(TRUE);
}

LRESULT CPostman::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ( IsWindowVisible(m_hWnd) && (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED))
	{
		CButtonUI* pMax = (CButtonUI*)m_PaintManager.FindControl(_T("BtnMax"));
		CButtonUI* pRestor = (CButtonUI*)m_PaintManager.FindControl(_T("BtnRestore"));
		pMax->SetVisible(wParam == SIZE_RESTORED);
		pRestor->SetVisible(wParam != SIZE_RESTORED);
	}

	return WindowImplBase::OnSize(uMsg,wParam,lParam,bHandled);
}

void CPostman::OnSize(TNotifyUI& msg)
{
	//目前不能根据，依赖控件位置变化改变浮动控件位置，BUG
	if (msg.pSender == m_PaintManager.GetRoot())
		ShowBar(m_bShowBar);
}

void CPostman::ShowBar(BOOL bShowBar)
{
	CControlUI* pFunc = m_PaintManager.FindControl(_T("Func"));
	pFunc->SetVisible(bShowBar != FALSE);

	CButtonUI* pHide = (CButtonUI*)m_PaintManager.FindControl(_T("BtnHide"));
	CButtonUI* pShow = (CButtonUI*)m_PaintManager.FindControl(_T("BtnShow"));
	pHide->SetVisible(bShowBar != FALSE);
	pShow->SetVisible(bShowBar == FALSE);

	CControlUI* pSplit = m_PaintManager.FindControl(_T("VSplit"));

	RECT rcPos = pFunc->GetPos();

	int nLeft = rcPos.left - 13;
	if (bShowBar == FALSE)
		nLeft = rcPos.right - 12;

	RECT rcCtrl = {nLeft, rcPos.top, nLeft+13, rcPos.bottom };
	pSplit->SetPos(rcCtrl);
}