#include "StdAfx.h"
#include "ZipRes.h"
#include "resource.h"

CZipRes::CZipRes()
{
	
}

CZipRes::~CZipRes()
{

}

DUI_BEGIN_MESSAGE_MAP(CZipRes,WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

LPCTSTR CZipRes::GetWindowClassName() const
{
	return _T("DuiLib::CZipRes");
}

CDuiString CZipRes::GetSkinFile()
{
	return _T("Installer.xml");
}

CDuiString CZipRes::GetZIPFileName() const
{
	return _T("Installer.zip");
}

CDuiString CZipRes::GetSkinFolder()
{
	return _T("");
}

UILIB_RESOURCETYPE CZipRes::GetResourceType() const
{
	return UILIB_ZIP;
}

LPCTSTR CZipRes::GetResourceID() const  
{  
       //return MAKEINTRESOURCE(IDR_ZIP_SKIN);  
	return NULL;
};

void CZipRes::Notify(TNotifyUI& msg)
{
	CDuiString strCtlName = msg.pSender->GetName();

	if (msg.sType == _T("click"))
	{
		if (strCtlName == _T("BtnClose"))
			PostQuitMessage(0);
		else if (strCtlName == _T("BtnMin"))
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		else
		{
			CTabLayoutUI* pWizardTab = (CTabLayoutUI*)m_PaintManager.FindControl(_T("WizardTab"));
			if (strCtlName == _T("BtnAccept") || strCtlName == _T("BtnNext"))
				pWizardTab->SelectItem(pWizardTab->GetCurSel()+1);
			else if (strCtlName == _T("BtnBack"))
				pWizardTab->SelectItem(pWizardTab->GetCurSel()-1);
			else if (strCtlName == _T("BtnCancel"))
				PostQuitMessage(0);
			else if (strCtlName == _T("BtnComplete"))
				PostQuitMessage(0);
		}
	}
	

	if (msg.sType == _T("setfocus"))
	{
		CTabLayoutUI* pControl = (CTabLayoutUI*)m_PaintManager.FindControl(_T("DiskTabLayout"));
		if (strCtlName == _T("LoadDisk"))
			pControl->SelectItem(0);
		else if (strCtlName == _T("NewDisk"))
			pControl->SelectItem(1);
		else if (strCtlName == _T("UnloadDisk"))
			pControl->SelectItem(2);
	}
}

CControlUI* CZipRes::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CZipRes::InitWindow()
{
	SetIcon(IDI_SMALL);
}

void CZipRes::OnClick(DuiLib::TNotifyUI &msg)
{

}

LRESULT CZipRes::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::OnCreate(uMsg,wParam,lParam,bHandled);
}

LRESULT CZipRes::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(uMsg)
	{
		case WM_MENUCLICK:
				OnMenuItemClick((CControlUI*)lParam);
			break;
		default:
			bHandled = FALSE;
	}
	return TRUE;
}

void CZipRes::OnMenuItemClick(CControlUI* pControl)
{
	ASSERT(pControl);
}
