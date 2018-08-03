#include "StdAfx.h"
#include "SignTool.h"
#include "Resource.h"
#include "HighClass.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Security/CryptHelper.h"
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
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
		OnItemSelect(msg);
	else if (msg.sType == DUI_MSGTYPE_MENU)
		OnMenu(msg);
}

LRESULT CSignTool::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	LRESULT bRes = FALSE;

	switch(uMsg)
	{
		case WM_MENUCLICK:
				bRes = OnMenuClick(wParam, lParam);
			break;
		default:
			bHandled = FALSE;
	}

	if (bHandled) return bRes;

	return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
}

void CSignTool::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	COptionUI* pTabSwitch = (COptionUI*)m_PaintManager.FindControl(_T("OptionUI3"));
	pTabSwitch->Selected(true);
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
	else if (strControlName == _T("BtnKeyPath"))
		OnPrivateKeyPath(msg, _T("PrivateKeyPath"));
	else if (strControlName == _T("BtnCreatePrivateKey"))
		OnCreatePrivateKey(msg);
	else if (strControlName == _T("BtnReqPath"))
		OnReqFilePath(msg, _T("ReqPath"));
	else if (strControlName == _T("BtnCreateReqFile"))
		OnCreateReqFile(msg);
	else if (strControlName == _T("BtnSignKeyPath"))
		OnPrivateKeyPath(msg, _T("SignPrivateKeyPath"));
	else if (strControlName == _T("BtnSignReqPath"))
		OnReqFilePath(msg, _T("SignReqPath"));
	else if (strControlName == _T("BtnSignCertPath"))
		OnCertFilePath(msg, _T("SignCertPath"));
	else if (strControlName == _T("BtnSignCACertPath"))
		OnCertFilePath(msg, _T("SignCACertPath"));
	else if (strControlName == _T("BtnSignCAPrivateKeyPath"))
		OnPrivateKeyPath(msg, _T("SignCAPrivateKeyPath"));
	else if (strControlName == _T("BtnSignCert"))
		OnSignCert(msg);
	else if (strControlName == _T("BtnSelfCertPath"))
		OnCertFilePath(msg, _T("SelfCertPath"));
	else if (strControlName == _T("BtnSelfKeyPath"))
		OnPrivateKeyPath(msg, _T("SelfPrivateKeyPath"));
	else if (strControlName == _T("BtnSelfReqPath"))
		OnPrivateKeyPath(msg, _T("SelfReqPath"));
	else if (strControlName == _T("BtnSelfSign"))
		OnCertSelfSign(msg);
	else if (strControlName == _T("BtnUpdate"))
		OnUpdateCertName(msg);
}

void CSignTool::OnCreatePrivateKey(TNotifyUI& msg)
{
	CDuiString strCmdParam = _T("genrsa");
	CEditUI2* pKeyFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("PrivateKeyPath"));
	CComboUI* pEncryptLen = (CComboUI*)m_PaintManager.FindControl(_T("EncryptLen"));
	CComboUI* pEncryptType = (CComboUI*)m_PaintManager.FindControl(_T("EncryptType"));
	CEditUI2* pEncryptPWD = (CEditUI2*)m_PaintManager.FindControl(_T("EncryptPWD"));
	if (pEncryptType->GetCurSel() != 0)
		strCmdParam.AppendFormat(_T(" %s -passout pass:%s"), (LPCTSTR)pEncryptType->GetUserData(), (LPCTSTR)pEncryptPWD->GetText());
	// 使用参数 控制私钥文件存放路径和私钥长度
	strCmdParam.AppendFormat(_T(" -out %s %s"), (LPCTSTR)pKeyFilePath->GetText(), (LPCTSTR)pEncryptLen->GetText());

	ShellExecute(m_hWnd, _T("open"), m_szOPENSSLPath, strCmdParam, NULL, SW_HIDE);
}

void CSignTool::OnCreateReqFile(TNotifyUI& msg)
{
	CDuiString strCmdParam = _T("req -new");

	CEditUI2* pPrivateKey = (CEditUI2*)m_PaintManager.FindControl(_T("PrivateKeyPath"));
	strCmdParam.AppendFormat(_T(" -key \"%s\""), pPrivateKey->GetText());

	CEditUI2* pReqFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("ReqPath"));
	if (pReqFilePath != NULL)
		strCmdParam.AppendFormat(_T(" -out \"%s\""), pReqFilePath->GetText());
	
	CDuiString strSubject;
	CEditUI2* pCountry = (CEditUI2*)m_PaintManager.FindControl(_T("Country"));
	if (pCountry != NULL)
		strSubject.AppendFormat(_T("/C=%s"), pCountry->GetText());

	CEditUI2* pProvince = (CEditUI2*)m_PaintManager.FindControl(_T("Province"));
	if (pProvince != NULL)
		strSubject.AppendFormat(_T("/ST=%s"), pProvince->GetText());

	CEditUI2* pCity = (CEditUI2*)m_PaintManager.FindControl(_T("City"));
	if (pCity != NULL)
		strSubject.AppendFormat(_T("/L=%s"), pCity->GetText());

	CEditUI2* pCompany = (CEditUI2*)m_PaintManager.FindControl(_T("Company"));
	if (pCompany != NULL)
		strSubject.AppendFormat(_T("/O=%s"),pCompany->GetText());

	CEditUI2* pOrgName = (CEditUI2*)m_PaintManager.FindControl(_T("OrgName"));
	if (pOrgName != NULL)
		strSubject.AppendFormat(_T("/OU=%s"), pOrgName->GetText());
	
	CEditUI2* pCommonName = (CEditUI2*)m_PaintManager.FindControl(_T("CommonName"));
	if (pCommonName != NULL)
		strSubject.AppendFormat(_T("/CN=%s"), pCommonName->GetText());

	strSubject.AppendFormat(_T("/emailAddress=yy@vivo.com"));

	//LPCTSTR lpszSubject = _T("/C=CN/ST=SC/L=CD/O=Lonsin Ltd/OU=dev/CN=www.lonsin.biz/emailAddress=yy@vivo.com");
	strCmdParam.AppendFormat(_T(" -subj \"%s\""), strSubject);
	strCmdParam.AppendFormat(_T(" -config %s"), m_szOpenSSLConfigPath);

	ShellExecute(m_hWnd, _T("open"), m_szOPENSSLPath, strCmdParam, NULL, SW_HIDE);
}

void CSignTool::OnCertSelfSign(TNotifyUI& msg)
{
	// 获取证书名称
	CEditUI2* pCertName = (CEditUI2*)m_PaintManager.FindControl(_T("SelfCertName"));

	// 获取密钥长度
	CComboUI* pEncryptLen = (CComboUI*)m_PaintManager.FindControl(_T("SelfKeyLen"));
	int nSelect = pEncryptLen->GetCurSel();
	DWORD dwKeyBit = (nSelect + 1) * 1024;

	// 获取证书类型
	CComboUI* pCertType = (CComboUI*)m_PaintManager.FindControl(_T("SelfCertType"));
	// 加密证书:1，签名证书:2
	CERT_TYPE CertType = (CERT_TYPE)(pCertType->GetCurSel() + 1);

	CEditUI2* pKeyName = (CEditUI2*)m_PaintManager.FindControl(_T("SelfKeyName"));
	//if (pKeyName != NULL)
	//	strCmdParam.AppendFormat(_T(" -sk %s"), pKeyName->GetText());
//
//	CEditUI2* pStartTime = (CEditUI2*)m_PaintManager.FindControl(_T("SelfStartTime"));
//	if (pStartTime != NULL)
//		strCmdParam.AppendFormat(_T(" -b %s"), pStartTime->GetText());
//
//	CEditUI2* pEndTime = (CEditUI2*)m_PaintManager.FindControl(_T("SelfEndTime"));
//	if (pEndTime != NULL)
//		strCmdParam.AppendFormat(_T(" -e %s"), pEndTime->GetText());
//
	CComboUI* pCertAlg = (CComboUI*)m_PaintManager.FindControl(_T("SelfCertAlg"));
	SIGNATURE_ALGORITHM SignAlg = (SIGNATURE_ALGORITHM)pCertAlg->GetCurSel();

	LPCWSTR lpszStoreName[] = {L"My", L"Root"};
	CComboUI* pCertSave = (CComboUI*)m_PaintManager.FindControl(_T("SelfCertSave"));
	int nStoreItem = pCertSave->GetCurSel();

	CComboUI* pSaveLocal = (CComboUI*)m_PaintManager.FindControl(_T("SelfCertSaveLocal"));
	CERT_LOCALTION CertSaveLocal = (CERT_LOCALTION)pSaveLocal->GetCurSel();
	
	CCryptHelper WinCrypt(CertType);
	WinCrypt.SetSignatureAlgorithm(SignAlg);
	WinCrypt.OpenContainer(NULL, NULL);
	WinCrypt.GenericKey(dwKeyBit);

	CString strSubjectName = pCertName->GetText();
	// 判断是否带有通用名称字段标签，如果没有自动添加
	if (strSubjectName.Find(_T("CN=")) == -1)
		strSubjectName.Insert(0,_T("CN="));

	SYSTEMTIME EndTime;
	GetSystemTime(&EndTime);
	EndTime.wYear += 5;
	WinCrypt.CreateSelfSignedCertificate(strSubjectName, NULL, &EndTime);
	
	if (nStoreItem < 2)
		WinCrypt.AddCertificateToStore(CertSaveLocal, lpszStoreName[nStoreItem]);
}

void CSignTool::OnUpdateCertName(TNotifyUI& msg)
{
	CVerticalLayoutUI* pHighClass = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("HighClass"));
	pHighClass->SetVisible(false);
}

void CSignTool::OnSelectChanged(TNotifyUI& msg)
{
	CDuiString strControlName = msg.pSender->GetName();
	if (msg.pSender->GetParent()->GetName() == _T("TabSwitch"))
	{
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("TabWizard"));
		if (pTabLayout == NULL)
			return;

		UINT nIndex = _ttoi(msg.pSender->GetUserData());
		pTabLayout->SelectItem(nIndex);
	}
}

void CSignTool::OnItemSelect(TNotifyUI& msg)
{
	CDuiString strControlName = msg.pSender->GetName();
	if (strControlName == _T("EncryptType"))
	{
		CComboUI* pEncryptType = (CComboUI*)msg.pSender->GetInterface(DUI_CTR_COMBO);
		CEditUI2* pEncryptPWD = (CEditUI2*)m_PaintManager.FindControl(_T("EncryptPWD"));
		if (pEncryptType->GetCurSel() == 0) {
			pEncryptPWD->SetText(_T(""));
			pEncryptPWD->SetEnabled(false);
		} else {
			pEncryptPWD->SetEnabled(true);
		}
	}
}

void CSignTool::OnMenu(TNotifyUI& msg)
{
	CDuiString strControlName = msg.pSender->GetName();
	if (strControlName == _T("SelfCertName"))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		CDuiPoint pt;
		GetCursorPos(&pt);
		STRINGorID strXmlFile(_T("CertNameMenu.xml"));
		pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);
	}
}

void CSignTool::OnSave(TNotifyUI& msg)
{
	
}

void CSignTool::OnPrivateKeyPath(TNotifyUI& msg, LPCTSTR lpszPrivateKeyName)
{
	TCHAR szFilePath[MAX_PATH] = {0};

	/*CEditUI2* pCertName = (CEditUI2*)m_PaintManager.FindControl(_T("CertName"));
	if (pCertName != NULL && pCertName->IsWaterMode() == FALSE)
		_stprintf_s(szFilePath, _countof(szFilePath), _T("%s.cer"), pCertName->GetText());*/
	_stprintf_s(szFilePath, _T("%s"), _T("C:\\bin\\key\\rsa_private.key"));

	OPENFILENAME SaveFileName = {0};
	SaveFileName.lStructSize		= sizeof(SaveFileName); 
	SaveFileName.hwndOwner		= m_hWnd; 
	SaveFileName.lpstrFile			= szFilePath; 
	SaveFileName.nMaxFile			= _countof(szFilePath);
	SaveFileName.lpstrFilter			= _T("证书私钥文件\0*.key;*.pem\0\0"); 
	SaveFileName.nFilterIndex		= 1; 
	SaveFileName.lpstrFileTitle	= NULL; 
	SaveFileName.nMaxFileTitle	= 0; 
	SaveFileName.lpstrTitle			= _T("请选择私钥文件保存位置");
	SaveFileName.lpstrInitialDir	= NULL ; 
	SaveFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetSaveFileName(&SaveFileName) == FALSE)
		return ;

	CEditUI2* pSavePath = (CEditUI2*)m_PaintManager.FindControl(lpszPrivateKeyName);
	if (pSavePath != NULL)
		pSavePath->SetText(szFilePath);
}

void CSignTool::OnReqFilePath(TNotifyUI& msg, LPCTSTR lpszReqName)
{
	TCHAR szFilePath[MAX_PATH] = {0};

	/*CEditUI2* pCertName = (CEditUI2*)m_PaintManager.FindControl(_T("CertName"));
	if (pCertName != NULL && pCertName->IsWaterMode() == FALSE)
		_stprintf_s(szFilePath, _countof(szFilePath), _T("%s.cer"), pCertName->GetText());*/
	_stprintf_s(szFilePath, _T("%s"), _T("C:\\bin\\key\\ca\\ca-req.csr"));

	OPENFILENAME SaveFileName = {0};
	SaveFileName.lStructSize		= sizeof(SaveFileName); 
	SaveFileName.hwndOwner		= m_hWnd; 
	SaveFileName.lpstrFile			= szFilePath; 
	SaveFileName.nMaxFile			= _countof(szFilePath);
	SaveFileName.lpstrFilter			= _T("证书请求文件\0*.csr;*.pem\0\0"); 
	SaveFileName.nFilterIndex		= 1; 
	SaveFileName.lpstrFileTitle	= NULL; 
	SaveFileName.nMaxFileTitle	= 0; 
	SaveFileName.lpstrTitle			= _T("请选择证书请求文件保存位置");
	SaveFileName.lpstrInitialDir	= NULL ; 
	SaveFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetSaveFileName(&SaveFileName) == FALSE)
		return ;

	CEditUI2* pSavePath = (CEditUI2*)m_PaintManager.FindControl(lpszReqName);
	if (pSavePath != NULL)
		pSavePath->SetText(szFilePath);
}

void CSignTool::OnCertFilePath(TNotifyUI& msg, LPCTSTR lpszCertName)
{
	TCHAR szFilePath[MAX_PATH] = {0};

	/*CEditUI2* pCertName = (CEditUI2*)m_PaintManager.FindControl(_T("CertName"));
	if (pCertName != NULL && pCertName->IsWaterMode() == FALSE)
		_stprintf_s(szFilePath, _countof(szFilePath), _T("%s.cer"), pCertName->GetText());*/
	_stprintf_s(szFilePath, _T("%s"), _T("C:\\bin\\key\\ca\\ca-cert.cer"));

	OPENFILENAME SaveFileName = {0};
	SaveFileName.lStructSize		= sizeof(SaveFileName); 
	SaveFileName.hwndOwner		= m_hWnd; 
	SaveFileName.lpstrFile			= szFilePath; 
	SaveFileName.nMaxFile			= _countof(szFilePath);
	SaveFileName.lpstrFilter			= _T("证书文件\0*.cer;*.pem\0\0"); 
	SaveFileName.nFilterIndex		= 1; 
	SaveFileName.lpstrFileTitle	= NULL; 
	SaveFileName.nMaxFileTitle	= 0; 
	SaveFileName.lpstrTitle			= _T("请选择证书文件保存位置");
	SaveFileName.lpstrInitialDir	= NULL ; 
	SaveFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetSaveFileName(&SaveFileName) == FALSE)
		return ;

	CEditUI2* pSavePath = (CEditUI2*)m_PaintManager.FindControl(lpszCertName);
	if (pSavePath != NULL)
		pSavePath->SetText(szFilePath);
}

void CSignTool::OnSignCert(TNotifyUI& msg)
{
	CDuiString strCmdParam = _T("x509 -req");

	CEditUI2* pPrivateKey = (CEditUI2*)m_PaintManager.FindControl(_T("SignPrivateKeyPath"));
	strCmdParam.AppendFormat(_T(" -signkey \"%s\""), pPrivateKey->GetText());

	CEditUI2* pReqFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("SignReqPath"));
	if (pReqFilePath != NULL)
		strCmdParam.AppendFormat(_T(" -in \"%s\""), pReqFilePath->GetText());

	CEditUI2* pCertFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("SignCertPath"));
	if (pCertFilePath != NULL)
		strCmdParam.AppendFormat(_T(" -out \"%s\""), pCertFilePath->GetText());

	CEditUI2* pCACertFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("SignCACertPath"));
	if (pCACertFilePath != NULL)
		strCmdParam.AppendFormat(_T(" -CA \"%s\""), pCACertFilePath->GetText());

	CEditUI2* pCAPrivateKey = (CEditUI2*)m_PaintManager.FindControl(_T("SignCAPrivateKeyPath"));
	if (pCAPrivateKey != NULL)
		strCmdParam.AppendFormat(_T(" -CAkey \"%s\""), pCAPrivateKey->GetText());

	strCmdParam.AppendFormat(_T(" -CAcreateserial -days 3650"));

	ShellExecute(m_hWnd, _T("open"), m_szOPENSSLPath, strCmdParam, NULL, SW_HIDE);
}

LRESULT CSignTool::OnMenuClick(WPARAM wParam, LPARAM lParam)
{
	CControlUI* pControl = (CControlUI*)lParam;
	ASSERT(pControl);
	if (pControl == NULL)
		return FALSE;

	if (pControl->GetName() == _T("MenuHighClass"))
	{
		CEditUI2* pCertName = (CEditUI2*)m_PaintManager.FindControl(_T("SelfCertName"));
		CHighClass* pHighClass = new CHighClass(m_hWnd, pCertName->GetText());
		pHighClass->ShowModal();
		CDuiString strCertNameInfo = pHighClass->GetCertNameInfo();
		if (strCertNameInfo.IsEmpty() == FALSE)
			pCertName->SetText(strCertNameInfo);

		delete pHighClass;
	}

	return TRUE;
}