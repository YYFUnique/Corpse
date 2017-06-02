#include "StdAfx.h"
#include "QRDlg.h"
#include "QRTool.h"
#include <CommDlg.h>
#define OPEN_FILE_TITLE	_T("请选择需要嵌入到二维码中心的图片")

CQRDlg::CQRDlg(HWND hParent)
{
	m_hParent = hParent;
	Create(hParent,_T("制作二维码"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CQRDlg::~CQRDlg()
{

}

void CQRDlg::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CQRDlg::GetWindowClassName() const
{
	return _T("QrCodeDlg");
}

UILIB_RESOURCETYPE CQRDlg::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CQRDlg::GetSkinFile()
{
	return _T("QRDlg.xml");
}

CDuiString CQRDlg::GetSkinFolder()
{
	return _T("QRTool");
}

void CQRDlg::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CQRDlg::InitWindow()
{
	RECT rcWnd;
	GetWindowRect(m_hParent,&rcWnd);

	SetWindowPos(m_hWnd, HWND_NOTOPMOST, rcWnd.right-6, rcWnd.top,  0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
}

void CQRDlg::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))	
		Close();
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnUpdate")))
		OnUpdateQRCode(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnLogo")))
		OnLookupFile(msg);
}

void CQRDlg::OnUpdateQRCode(TNotifyUI& msg)
{
	BYTE lpData[4096] = {0};
	QRCodeItem* pQRCodeInfo = (QRCodeItem*)lpData;
	pQRCodeInfo->strQrCode.Empty();
	CComboUI* pQRLevel = (CComboUI*)m_PaintManager.FindControl(_T("Level"));
	pQRCodeInfo->dwLevel = pQRLevel->GetCurSel();
	pQRCodeInfo->dwSize = _ttoi(m_PaintManager.FindControl(_T("Size"))->GetText());
	pQRCodeInfo->dwMag = _ttoi(m_PaintManager.FindControl(_T("Atom"))->GetText());
	pQRCodeInfo->dwBorder = _ttoi(m_PaintManager.FindControl(_T("Border"))->GetText());
	pQRCodeInfo->strLogoFile = m_PaintManager.FindControl(_T("EditLogo"))->GetText();
	pQRCodeInfo->strQrCode  = m_PaintManager.FindControl(_T("Context"))->GetText();
	::SendMessage(m_hParent, WM_QRCODEITEM_INFO, (WPARAM)pQRCodeInfo, NULL);
}

void CQRDlg::OnLookupFile(TNotifyUI& msg)
{
	TCHAR szLogoFilePath[MAX_PATH] = {0};
	OPENFILENAME OFNFileName;
	ZeroMemory(&OFNFileName ,sizeof(OFNFileName)); 
	OFNFileName.lStructSize		= sizeof(OFNFileName); 
	OFNFileName.hwndOwner		= m_hWnd; 
	OFNFileName.lpstrFile			= szLogoFilePath ; 
	OFNFileName.nMaxFile			= _countof(szLogoFilePath);
	OFNFileName.lpstrFilter			= _T("位图文件\0*.bmp\0PNG文件\0*.png\0"); 
	OFNFileName.nFilterIndex		= 1; 
	OFNFileName.lpstrFileTitle	= NULL; 
	OFNFileName.nMaxFileTitle	= 0;
	OFNFileName.lpstrTitle			= OPEN_FILE_TITLE;
	OFNFileName.lpstrInitialDir	= NULL; 
	OFNFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 
	if (GetOpenFileName(&OFNFileName))
	{
		CEditUI2* pLogoPath = (CEditUI2*)m_PaintManager.FindControl(_T("EditLogo"));
		if (pLogoPath)
			pLogoPath->SetText(szLogoFilePath);
	}
}