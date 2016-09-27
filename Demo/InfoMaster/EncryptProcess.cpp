#include "StdAfx.h"
#include "EncryptProcess.h"
#include "Resource.h"

CEncryptProcess::CEncryptProcess(HWND hParent)
{
	Create(hParent, _T("EncryptProcess"), WS_POPUP|UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0);
}

CEncryptProcess::~CEncryptProcess()
{

}

void CEncryptProcess::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CEncryptProcess::GetWindowClassName() const
{
	return _T("TxProcess");
}

UILIB_RESOURCETYPE CEncryptProcess::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIPRESOURCE;
#endif
}

LPCTSTR CEncryptProcess::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_SKIN);
}

CDuiString CEncryptProcess::GetSkinFile()
{
	return _T("Process.xml");
}

CDuiString CEncryptProcess::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("InfoMaster");
#else
	return _T("");
#endif
}

void CEncryptProcess::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CEncryptProcess::InitWindow()
{
	CenterWindow();
}

void CEncryptProcess::SetEncryptProcess(UINT nProcess)
{
	CProgressUI* pEncryptProcess = (CProgressUI*)m_PaintManager.FindControl(_T("rate"));
	if (pEncryptProcess == NULL)
		return ;

	pEncryptProcess->SetValue(nProcess);
}

/*LRESULT CEncryptProcess::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}*/

void CEncryptProcess::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
		Close(IDOK);
}