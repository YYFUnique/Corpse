#include "StdAfx.h"
#include "Resource.h"
#include "OEMInfoModify.h"

#include "DllCore/Utils/Registry.h"
#include "DllCore/Utils/OsInfo.h"
#include <CommDlg.h>
#define OEM_REG_PATH		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OemInformation")

COEMInfoModify::COEMInfoModify()
{

}

COEMInfoModify::~COEMInfoModify()
{

}

LPCTSTR COEMInfoModify::GetWindowClassName() const
{
	return _T("OEMInfoModify");
}

UILIB_RESOURCETYPE COEMInfoModify::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#elif defined LS_STATIC_LIB_CALL
	return UILIB_ZIPRESOURCE;
#else
	return UILIB_ZIP;
#endif
}

CDuiString COEMInfoModify::GetZIPFileName() const
{
	return _T("OEM.zip");
}

CDuiString COEMInfoModify::GetSkinFile()
{
	return _T("OEM.xml");
}

CDuiString COEMInfoModify::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("OEM");
#else
	return _T("");
#endif
}

LPCTSTR COEMInfoModify::GetResourceID() const
{
	return MAKEINTRESOURCE(IDR_ZIPRES);
}

void COEMInfoModify::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void COEMInfoModify::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	do 
	{
		DWORD dwKeyValue = KEY_READ;
		if (OsIsWow64Process())
			dwKeyValue = KEY_READ_WOW64_VALUE;

		//获取制造商名称
		CEditUI2* pManufacturer = (CEditUI2*)m_PaintManager.FindControl(_T("Manufacturer"));
		if (pManufacturer == NULL)
			break;

		CString strManufacturer;
		LsRegQueryValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Manufacturer"), strManufacturer, dwKeyValue);
		pManufacturer->SetText(strManufacturer);

		//获取产品型号
		CEditUI2* pModel = (CEditUI2*)m_PaintManager.FindControl(_T("Model"));
		if (pModel == NULL)
			break;

		CString strModel;
		LsRegQueryValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Model"), strModel, dwKeyValue);
		pModel->SetText(strModel);

		//获取LOGO图片
		CEditUI2* pLogo = (CEditUI2*)m_PaintManager.FindControl(_T("EditPackPath"));
		if (pLogo == NULL)
			break;

		CString strLogo;
		LsRegQueryValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Logo"), strLogo, dwKeyValue);
		pLogo->SetText(strLogo);

		//获取制造商联系电话
		CEditUI2* pSupportPhone = (CEditUI2*)m_PaintManager.FindControl(_T("SupportPhone"));
		if (pSupportPhone == NULL)
			break;

		CString strSupportPhone;
		LsRegQueryValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportPhone"), strSupportPhone, dwKeyValue);
		pSupportPhone->SetText(strSupportPhone);

		//获取制造商服务时间
		CEditUI2* pSupportTime = (CEditUI2*)m_PaintManager.FindControl(_T("SupportTime"));
		if (pSupportTime == NULL)
			break;

		CString strSupportTime;
		LsRegQueryValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportHours"), strSupportTime, dwKeyValue);
		pSupportTime->SetText(strSupportTime);

		CEditUI2* pSupportURL = (CEditUI2*)m_PaintManager.FindControl(_T("SupportURL"));
		if (pSupportURL == NULL)
			break;

		CString strSupportURL;
		LsRegQueryValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportURL"), strSupportURL, dwKeyValue);
		pSupportURL->SetText(strSupportURL);

	} while (FALSE);
}

void COEMInfoModify::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void COEMInfoModify::OnClick(TNotifyUI& msg)
{
	CDuiString strControlName = msg.pSender->GetName();
	if (strControlName == _T("BtnClose"))
	{
		Close(IDOK);
		PostQuitMessage(0);
	}
	else if (strControlName == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (strControlName == _T("BtnSave"))
		OnSaveOEMInfo(msg);
	else if (strControlName == _T("BtnPackPath"))
		OnGetLogoFilePath(msg);
	else if (strControlName == _T("BtnDefault"))
		OnSetDefault(msg);
}

void COEMInfoModify::OnGetLogoFilePath(TNotifyUI& msg)
{
	TCHAR szLogoFilePath[MAX_PATH] = {0};
	OPENFILENAME OFNFileName;
	ZeroMemory(&OFNFileName ,sizeof(OFNFileName)); 
	OFNFileName.lStructSize		= sizeof(OFNFileName); 
	OFNFileName.hwndOwner		= m_hWnd; 
	OFNFileName.lpstrFile			= szLogoFilePath ; 
	OFNFileName.nMaxFile			= _countof(szLogoFilePath);
	OFNFileName.lpstrFilter			= _T("位图文件\0*.bmp\0\0"); 
	OFNFileName.nFilterIndex		= 1; 
	OFNFileName.lpstrFileTitle	= NULL; 
	OFNFileName.nMaxFileTitle	= 0;
	OFNFileName.lpstrTitle			= _T("请选择OEM厂商LOGO");
	OFNFileName.lpstrInitialDir	= NULL; 
	OFNFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 
	if (GetOpenFileName(&OFNFileName))
	{
		CEditUI2* pLogo = (CEditUI2*)m_PaintManager.FindControl(_T("EditPackPath"));
		if (pLogo == NULL)
			return;

		pLogo->SetText(szLogoFilePath);
	}
}

void COEMInfoModify::OnSaveOEMInfo(TNotifyUI& msg)
{
	do 
	{
		//获取制造商名称
		CEditUI2* pManufacturer = (CEditUI2*)m_PaintManager.FindControl(_T("Manufacturer"));
		if (pManufacturer == NULL)
			break;

		//获取产品型号
		CEditUI2* pModel = (CEditUI2*)m_PaintManager.FindControl(_T("Model"));
		if (pModel == NULL)
			break;

		//获取LOGO图片
		CEditUI2* pLogo = (CEditUI2*)m_PaintManager.FindControl(_T("EditPackPath"));
		if (pLogo == NULL)
			break;

		//获取制造商联系电话
		CEditUI2* pSupportPhone = (CEditUI2*)m_PaintManager.FindControl(_T("SupportPhone"));
		if (pSupportPhone == NULL)
			break;
		
		//获取制造商服务时间
		CEditUI2* pSupportTime = (CEditUI2*)m_PaintManager.FindControl(_T("SupportTime"));
		if (pSupportTime == NULL)
			break;

		CEditUI2* pSupportURL = (CEditUI2*)m_PaintManager.FindControl(_T("SupportURL"));
		if (pSupportURL == NULL)
			break;

		DWORD dwKeyValue = KEY_SET_VALUE;
		if (OsIsWow64Process())
			dwKeyValue = KEY_SET_WOW64_VALUE;

		//设置制造商名称
		LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Manufacturer"), pManufacturer->GetText(), dwKeyValue);
		//设置产品型号
		LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Model"), pModel->GetText(), dwKeyValue);
		//设置制造商联系电话
		LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportPhone"), pSupportPhone->GetText(), dwKeyValue);
		//设置制造商服务时间
		LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportHours"), pSupportTime->GetText(), dwKeyValue);
		//设置制造商服务网址
		LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportURL"), pSupportURL->GetText(), dwKeyValue);
		//设置制造商LOGO
		LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Logo"), pLogo->GetText(), dwKeyValue);

		MessageBox(m_hWnd, _T("保存成功！"), _T("提示") ,MB_OK|MB_ICONINFORMATION);
	} while (FALSE);
}

void COEMInfoModify::OnSetDefault(TNotifyUI& msg)
{
	DWORD dwKeyValue = KEY_SET_VALUE;
	if (OsIsWow64Process())
		dwKeyValue = KEY_SET_WOW64_VALUE;

	//设置制造商名称
	LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Manufacturer"), _T(""), dwKeyValue);
	//设置产品型号
	LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Model"), _T(""), dwKeyValue);
	//设置制造商联系电话
	LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportPhone"), _T(""), dwKeyValue);
	//设置制造商服务时间
	LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportHours"), _T(""), dwKeyValue);
	//设置制造商服务网址
	LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("SupportURL"), _T(""), dwKeyValue);
	//设置制造商LOGO
	LsRegSetValue(HKEY_LOCAL_MACHINE, OEM_REG_PATH, _T("Logo"), _T(""), dwKeyValue);

	MessageBox(m_hWnd, _T("还原成功！"), _T("提示") ,MB_OK|MB_ICONINFORMATION);
}