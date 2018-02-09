#include "StdAfx.h"
#include "SignTool.h"
#include "Resource.h"
#include "Encrypt.h"
#include "DllCore/Utils/ErrorInfo.h"
#include <CommDlg.h>
CSignTool::CSignTool()
{

}

CSignTool::~CSignTool()
{

}

void CSignTool::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CSignTool::GetWindowClassName() const
{
	return _T("SignTool");
}

UILIB_RESOURCETYPE CSignTool::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CSignTool::GetSkinFile()
{
	return _T("SignTool.xml");
}

CDuiString CSignTool::GetSkinFolder()
{
	return _T("SignTool");
}

void CSignTool::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);
}

void CSignTool::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

void CSignTool::OnClick(TNotifyUI& msg)
{
	CDuiString strControlName = msg.pSender->GetName();
	if (strControlName == _T("BtnClose") || strControlName == _T("BtnCancle"))
	{
		Close(IDOK);
		PostQuitMessage(0);
	}
	else if (strControlName == _T("BtnMini"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (strControlName == _T("BtnSave"))
		OnSave(msg);
	else if (strControlName == _T("BtnLookup"))
		OnLookup(msg);
}

void CSignTool::OnSelectChanged(TNotifyUI& msg)
{
	CDuiString strControlName = msg.pSender->GetName();
	if (strControlName == _T("DerCode") || strControlName == _T("Base64Code"))
	{
		CEditUI2* pSavePath = (CEditUI2*)m_PaintManager.FindControl(_T("SavePath"));
		CButtonUI* pLookup = (CButtonUI*)m_PaintManager.FindControl(_T("BtnLookup"));
		if (pSavePath != NULL)
			pSavePath->SetEnabled(true);
		if (pLookup != NULL)
			pLookup->SetEnabled(true);
	}
	else if (strControlName == _T("CertCode"))
	{
		CEditUI2* pSavePath = (CEditUI2*)m_PaintManager.FindControl(_T("SavePath"));
		CButtonUI* pLookup = (CButtonUI*)m_PaintManager.FindControl(_T("BtnLookup"));
		if (pSavePath != NULL)
			pSavePath->SetEnabled(false);
		if (pLookup != NULL)
			pLookup->SetEnabled(false);
	}
}

void CSignTool::OnSave(TNotifyUI& msg)
{
	BOOL bSuccess = FALSE;

	do 
	{
		CEditUI2* pCertName = (CEditUI2*)m_PaintManager.FindControl(_T("CertName"));
		if (pCertName->IsWaterMode())
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("证书名称不能为空，请重新输入证书名称！"));
			break;
		}

		CEditUI2* pStartTime = (CEditUI2*)m_PaintManager.FindControl(_T("StartTime"));
		if (pStartTime == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("证书开始时间不能为空，请重新选择！"));
			break;
		}

		CEditUI2* pEndTime = (CEditUI2*)m_PaintManager.FindControl(_T("EndTime"));
		if (pEndTime == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("证书结束时间不能为空，请重新选择！"));
			break;
		}
		COptionUI* pSaveType = (COptionUI*)m_PaintManager.FindControl(_T("CertCode"));

		CEditUI2* pSavePath = (CEditUI2*)m_PaintManager.FindControl(_T("SavePath"));
		if (pSaveType->IsSelected() == FALSE && pSavePath->IsWaterMode())
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("证书保存路径不能为空，请重新选择！"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
	{
		MessageBox(m_hWnd, GetThreadErrorInfoString(), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CEditUI2* pCertName = (CEditUI2*)m_PaintManager.FindControl(_T("CertName"));
	CreateSelfSignedCertificate(pCertName->GetText());

}

void CSignTool::OnLookup(TNotifyUI& msg)
{
	TCHAR szFilePath[MAX_PATH] = {0};

	CEditUI2* pCertName = (CEditUI2*)m_PaintManager.FindControl(_T("CertName"));
	if (pCertName != NULL && pCertName->IsWaterMode() == FALSE)
		_stprintf_s(szFilePath, _countof(szFilePath), _T("%s.cer"), pCertName->GetText());

	OPENFILENAME SaveFileName = {0};
	SaveFileName.lStructSize		= sizeof(SaveFileName); 
	SaveFileName.hwndOwner		= m_hWnd; 
	SaveFileName.lpstrFile			= szFilePath; 
	SaveFileName.nMaxFile			= _countof(szFilePath);
	SaveFileName.lpstrFilter			= _T("证书格式文件\0*.cer\0"); 
	SaveFileName.nFilterIndex		= 1; 
	SaveFileName.lpstrFileTitle	= NULL; 
	SaveFileName.nMaxFileTitle	= 0; 
	SaveFileName.lpstrTitle			= _T("请选择证书文件保存位置");
	SaveFileName.lpstrInitialDir	= NULL ; 
	SaveFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetSaveFileName(&SaveFileName) == FALSE)
		return ;

	CEditUI2* pSavePath = (CEditUI2*)m_PaintManager.FindControl(_T("SavePath"));
	if (pSavePath != NULL)
	{
		pSavePath->SetText(szFilePath);
	}
}