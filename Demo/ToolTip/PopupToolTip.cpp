#include "StdAfx.h"
#include "PopupToolTip.h"
#include "Resource.h"
#include "DllCore/Window/WinHelper.h"

CPopupToolTip::CPopupToolTip()
{
	Create(NULL, _T("Í¼±ê´°¿Ú"), WS_POPUP, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CPopupToolTip::~CPopupToolTip()
{

}

void CPopupToolTip::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CPopupToolTip::GetWindowClassName() const
{
	return _T("ToolTip");
}

UILIB_RESOURCETYPE CPopupToolTip::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIPRESOURCE;
#endif
}

LPCTSTR CPopupToolTip::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_SKIN);
}

CDuiString CPopupToolTip::GetZIPFileName() const
{
	return _T("ToolTip.zip");
}

CDuiString CPopupToolTip::GetSkinFile()
{
	return _T("PopupToolTip.xml");
}

CDuiString CPopupToolTip::GetSkinFolder()
{
	return _T("ToolTip");
}

void CPopupToolTip::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CPopupToolTip::InitWindow()
{

}

void CPopupToolTip::Show(BOOL bShow)
{
	ShowWindow(bShow != FALSE);
	if (bShow != FALSE)
		ShowDesktop();
}

void CPopupToolTip::SetTooltip(LPCTSTR lpszTooltip)
{
	CLabelUI* pTooltip = (CLabelUI*)m_PaintManager.FindControl(_T("tip"));
	if (pTooltip != NULL)
		pTooltip->SetText(lpszTooltip);
}

void CPopupToolTip::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
		Close(IDOK);
	else if (msg.pSender->GetName() == _T("BtnOK"))
		Show(FALSE);
}