#include "StdAfx.h"
#include "HighClass.h"
#include "DllCore/Utils/TextTools.h"

CHighClass::CHighClass(HWND hParent, LPCTSTR lpszCertName)
{
	m_strCertNameInfo = lpszCertName;
	Create(hParent, _T("HighClass"), WS_POPUP|UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0);
}

CHighClass::~CHighClass()
{

}

void CHighClass::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	//delete this;
}

LPCTSTR CHighClass::GetWindowClassName() const
{
	return _T("HighClass");
}

UILIB_RESOURCETYPE CHighClass::GetResourceType() const
{
#if _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

CDuiString CHighClass::GetSkinFile()
{
	return _T("HighClass.xml");
}

CDuiString CHighClass::GetSkinFolder()
{
#if _DEBUG
	return _T("SignTool");
#else
	return _T("");
#endif
}

void CHighClass::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CHighClass::InitWindow()
{
	CStdArray strNameArr;
	m_strCertNameInfo.Replace(_T(" "), _T(""));
	AnalyseFormattedTextInfoItem(m_strCertNameInfo, strNameArr, _T(','));
	CEditUI2* pCName = (CEditUI2*)m_PaintManager.FindControl(_T("CName"));
	CEditUI2* pCountryName = (CEditUI2*)m_PaintManager.FindControl(_T("CountryName"));
	CEditUI2* pProvinceName = (CEditUI2*)m_PaintManager.FindControl(_T("ProvinceName"));
	CEditUI2* pOrgName = (CEditUI2*)m_PaintManager.FindControl(_T("OrgName"));
	CEditUI2* pCityName = (CEditUI2*)m_PaintManager.FindControl(_T("CityName"));
	CEditUI2* pOrgUnitName = (CEditUI2*)m_PaintManager.FindControl(_T("OrgUnitName"));
	CEditUI2* pEmail = (CEditUI2*)m_PaintManager.FindControl(_T("Email"));

	if (strNameArr.IsEmpty() && m_strCertNameInfo.IsEmpty() == FALSE)
		pCName->SetText(m_strCertNameInfo);

	for (int n=0; n<strNameArr.GetCount(); ++n)
	{
		const CString& strNameInfo = strNameArr.GetAt(n);
		if (_tcsnicmp(strNameInfo, _T("CN="), 3) == 0)			// 证书通用名称
			pCName->SetText(strNameInfo.Mid(3));
		else if (_tcsnicmp(strNameInfo, _T("C="), 2) ==  0)		// 证书国家
			pCountryName->SetText(strNameInfo.Mid(2));
		else if (_tcsnicmp(strNameInfo, _T("S="), 2) == 0)		// 证书省份
			pProvinceName->SetText(strNameInfo.Mid(2));
		else if (_tcsnicmp(strNameInfo, _T("L="), 2) == 0)		// 城市
			pCityName->SetText(strNameInfo.Mid(2));
		else if (_tcsnicmp(strNameInfo, _T("O="), 2) == 0)		// 组织
			pOrgName->SetText(strNameInfo.Mid(2));
		else if (_tcsnicmp(strNameInfo, _T("OU="), 3) == 0)	// 部门
			pOrgUnitName->SetText(strNameInfo.Mid(3));
		else if (_tcsnicmp(strNameInfo, _T("E="), 2) == 0)		// 电邮
			pEmail->SetText(strNameInfo.Mid(2));
		else 
			pCName->SetText(strNameInfo);
	}

	//ShowWindow(true, true);
	CenterWindow();
	//SIZE size = m_PaintManager.GetInitSize();
	//SetWindowPos(m_hWnd, HWND_TOPMOST, size.cx-7, m_lpRectParent.top, size.cx, size.cy, SWP_SHOWWINDOW);
}

LRESULT CHighClass::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	return TRUE;
}

void CHighClass::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnUpdate"))
		OnUpdate();
}

void CHighClass::OnUpdate()
{
	CEditUI2* pCName = (CEditUI2*)m_PaintManager.FindControl(_T("CName"));
	CEditUI2* pCountryName = (CEditUI2*)m_PaintManager.FindControl(_T("CountryName"));
	CEditUI2* pProvinceName = (CEditUI2*)m_PaintManager.FindControl(_T("ProvinceName"));
	CEditUI2* pOrgName = (CEditUI2*)m_PaintManager.FindControl(_T("OrgName"));
	CEditUI2* pCityName = (CEditUI2*)m_PaintManager.FindControl(_T("CityName"));
	CEditUI2* pOrgUnitName = (CEditUI2*)m_PaintManager.FindControl(_T("OrgUnitName"));
	CEditUI2* pEmail = (CEditUI2*)m_PaintManager.FindControl(_T("Email"));

	m_strCertNameInfo.Empty();
	CDuiString strNameInfo = pCName->GetText();
	if (strNameInfo.IsEmpty() == FALSE)
		m_strCertNameInfo.AppendFormat(_T("CN=%s,"), (LPCTSTR)strNameInfo);

	strNameInfo = pCountryName->GetText();
	if (strNameInfo.IsEmpty() == FALSE)
		m_strCertNameInfo.AppendFormat(_T("C=%s,"), (LPCTSTR)strNameInfo);

	strNameInfo = pProvinceName->GetText();
	if (strNameInfo.IsEmpty() == FALSE)
		m_strCertNameInfo.AppendFormat(_T("S=%s,"), (LPCTSTR)strNameInfo);

	strNameInfo = pCityName->GetText();
	if (strNameInfo.IsEmpty() == FALSE)
		m_strCertNameInfo.AppendFormat(_T("L=%s,"), (LPCTSTR)strNameInfo);

	strNameInfo = pOrgName->GetText();
	if (strNameInfo.IsEmpty() == FALSE)
		m_strCertNameInfo.AppendFormat(_T("O=%s,"), (LPCTSTR)strNameInfo);

	strNameInfo = pOrgUnitName->GetText();
	if (strNameInfo.IsEmpty() == FALSE)
		m_strCertNameInfo.AppendFormat(_T("OU=%s,"), (LPCTSTR)strNameInfo);

	strNameInfo = pEmail->GetText();
	if (strNameInfo.IsEmpty() == FALSE)
		m_strCertNameInfo.AppendFormat(_T("E=%s,"), (LPCTSTR)strNameInfo);

	m_strCertNameInfo.TrimRight(_T(","));

	Close(IDOK);
}

CDuiString CHighClass::GetCertNameInfo() const
{
	return m_strCertNameInfo;
}

LRESULT CHighClass::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}