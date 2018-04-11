#include "stdafx.h"
#include "SkinPickerDialog.h"
#include "SkinPickerPictureItem.h"

CSkinPickerDialog::CSkinPickerDialog(LPCTSTR pszXMLPath, CPCHunter *pMainWnd)
	: m_strXMLPath(pszXMLPath)
	, m_pMainWnd(pMainWnd)
	, m_pSkinTab(NULL)
{

}

CSkinPickerDialog::~CSkinPickerDialog()
{

}


void CSkinPickerDialog::OnFinalMessage(HWND hWnd) 
{ 
	WindowImplBase::OnFinalMessage(hWnd);
}

CDuiString CSkinPickerDialog::GetSkinFile()
{
	return m_strXMLPath;
}

CDuiString CSkinPickerDialog::GetSkinFolder()
{
	return _T("PCHunter");
}

void CSkinPickerDialog::InitWindow()
{
 	SetBkColor(m_pMainWnd->GetBkColor());
 	SetBkImage(m_pMainWnd->GetBkImage());
	
	m_pSkinTab = (CAnimationTabLayoutUI*)m_PaintManager.FindControl(_T("Skin_Tab"));
	CenterWindow();
}

LPCTSTR CSkinPickerDialog::GetWindowClassName() const
{
	return _T("CSkinPickerDialog");
}

LRESULT CSkinPickerDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CSkinPickerDialog::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

CControlUI* CSkinPickerDialog::CreateControl(LPCTSTR pstrClass) 
{
	if (_tcsicmp(pstrClass,_T("SkinPikerPictureItem")) == 0)
		return new CSkinPikerPictureItemUI();
	return NULL;
}

DWORD CSkinPickerDialog::GetBkColor() const
{
	CControlUI* pBackground = m_PaintManager.FindControl(_T("HTitle"));
	if (pBackground != NULL)
		return pBackground->GetBkColor();
	return 0;
}

void CSkinPickerDialog::SetBkColor(DWORD dwBackColor)
{
	CControlUI* pBackground = m_PaintManager.FindControl(_T("HTitle"));
	if (pBackground != NULL)
	{
		pBackground->SetBkImage(_T(""));
		pBackground->SetBkColor(dwBackColor);
		pBackground->NeedUpdate();
	}
}

void CSkinPickerDialog::SetBkImage(CDuiString strBkImage)
{
	CControlUI* pBackground = m_PaintManager.FindControl(_T("HTitle"));
	if (pBackground != NULL)
	{
		TCHAR szBuf[MAX_PATH] = {0};
		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='%s' source='0,0,1000,30'"), strBkImage);
		pBackground->SetBkImage(szBuf);
	}
}

LPCTSTR CSkinPickerDialog::GetBkImage()
{
	CControlUI* pBackground = m_PaintManager.FindControl(_T("HTitle"));
	if (pBackground != NULL)
		return pBackground->GetBkImage();
	return _T("");
}

void CSkinPickerDialog::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);
}

void CSkinPickerDialog::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnClose")))
		Close();
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("Skin_BtnSkinTrans")))
		OnBtnTrans(msg,_T("ListTrans.xml"),SLIDER_TYPE_CHANGE_WNDTRANS);
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("Skin_BtnListTrans")))
		OnBtnTrans(msg,_T("ListTrans.xml"),SLIDER_TYPE_CHANGE_LISTTRANS);
	else if (msg.pSender->GetInterface(_T("SkinPikerPictureItem")) != NULL)
		OnChoseSkin(msg);
}

void CSkinPickerDialog::OnChoseSkin(TNotifyUI& msg)
{
	CSkinPikerPictureItemUI * pItem = static_cast<CSkinPikerPictureItemUI*>(msg.pSender);
	CDuiString strBkImage = pItem->GetBkImage();
	strBkImage.Replace(_T("small"), _T(""));
	SetBkImage(strBkImage);
	m_pMainWnd->SetBkImage(strBkImage);
}

void CSkinPickerDialog::OnBtnTrans(TNotifyUI& msg,LPCTSTR lpszFilePath,SLIDER_TYPE Type)
{
 	CPopupWidget* pWidget = new CPopupWidget;
 	CDuiPoint point;
 	RECT rc	= msg.pSender->GetPos();
 	point.x = rc.left;
 	point.y = rc.top;			
 	pWidget->Init(lpszFilePath, m_hWnd, m_pMainWnd->GetMainWndPaintManager(), point, Type, msg.pSender);

	return;
}

void CSkinPickerDialog::OnSelectChanged(TNotifyUI& msg)
{
	if( msg.pSender->GetName() == _T("Skin_OptOfficial")) {
		m_pSkinTab->SelectItem(0);
	}
	else if( msg.pSender->GetName() == _T("Skin_OptMy")) {
		m_pSkinTab->SelectItem(1);
	}
	else if( msg.pSender->GetName() == _T("Skin_OptSkinTrans")) {
		CControlUI* pBtnTrans = m_PaintManager.FindControl(_T("Skin_BtnSkinTrans"));
		if(static_cast<COptionUI*>(msg.pSender)->IsSelected())
		{
			pBtnTrans->SetEnabled(true);
		}
		else
		{
			pBtnTrans->SetEnabled(false);
			pBtnTrans->SetTag(NULL);
		}
	}
}