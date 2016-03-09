#include "../stdafx.h"
#include "../resource.h"
#include "SkinChange.h"
#include "../ControlEx/SkinFace.h"


CSkinChange::CSkinChange(LPCTSTR lpszBkFile,DWORD dwBkColor)
: m_strBkFile(lpszBkFile)
, m_dwBkColor(dwBkColor)
{
	Create(NULL,_T("¸ü»»½çÃæÆ¤·ô"), UI_WNDSTYLE_DIALOG,NULL, 0, 0, 0, 0);
	ShowWindow();
}

CSkinChange::~CSkinChange()
{

}

void CSkinChange::OnFinalMessage(HWND hWnd)
{
	RemoveObserver();
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CSkinChange::GetWindowClassName() const
{
	return _T("LsGuiFoundationSkin");
}

void CSkinChange::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

CControlUI* CSkinChange::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass,_T("SkinDescribe")) == 0)
		return new CSkinDescribeUI(m_PaintManager);
	return NULL;
}

BOOL CSkinChange::Receive(SkinChangedParam lParam)
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

void CSkinChange::InitWindow()
{
	CenterWindow();
	CControlUI* pControl = m_PaintManager.FindControl(_T("VLayoutTotal"));
	if (pControl)
	{
		pControl->SetBkImage(m_strBkFile);
		pControl->SetBkColor(m_dwBkColor);
	}

	SetIcon(IDI_MAINFRAME);

	InsertGroup();
}

CDuiString CSkinChange::GetSkinFile()
{
	return _T("SkinFace.xml");
}

CDuiString CSkinChange::GetSkinFolder()
{
	return _T("pcmaster");
}

LRESULT CSkinChange::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
		Close();
	else
		__super::ResponseDefaultKeyEvent(wParam);
	return TRUE;
}

void CSkinChange::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnClose")))
		Close();
}

void CSkinChange::InsertGroup()
{
	CTileListCtrlUI* pListCtrl = (CTileListCtrlUI*)m_PaintManager.FindControl(_T("FaceList"));
	if (pListCtrl == NULL)
		return;
	
	SkinFaceInfo SkinFace;
	for (int i= 0;i<10;++i)
	{
		SkinFace.strAuthor = _T("Unique");
		SkinFace.strFaceDes = _T("text");
		pListCtrl->AddNode((LPARAM)&SkinFace);
	}
}