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

	COptionUI* pTabSwitch = (COptionUI*)m_PaintManager.FindControl(_T("OptionUI2"));
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
	else if (strControlName == _T("BtnCreateReqFile"))
		OnBtnCreateReqFile(msg);
	else if (strControlName == _T("BtnSave"))
		OnSave(msg);
	else if (strControlName == _T("BtnKeyPath"))
		OnPrivateKeyPath(msg, _T("PrivateKeyPath"));
	else if (strControlName == _T("BtnReqPath"))
		OnReqFilePath(msg, _T("ReqPath"));
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
		OnBtnSignCert(msg);
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

void CSignTool::OnBtnCreateReqFile(TNotifyUI& msg)
{
	CEditUI2* pRequestFile = (CEditUI2*)m_PaintManager.FindControl(_T("ReqPath"));
	CDuiString strRequestFilePath = pRequestFile->GetText();
	if (strRequestFilePath.IsEmpty())
	{
		MessageBox(m_hWnd, _T("请选择请求文件存放位置！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		pRequestFile->SetFocus();
		return;
	}

	CDuiString strSubjectName;
	CEditUI2* pCName = (CEditUI2*)m_PaintManager.FindControl(_T("CommonName"));
	CDuiString strCName = pCName->GetText();
	if (strCName.IsEmpty())
	{
		MessageBox(m_hWnd, _T("请输入证书通用名称！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		pCName->SetFocus();
		return;
	}

	if (pCName != NULL)
		strSubjectName.Format(_T("CN=%s,"), strCName);

	CEditUI2* pCountryName = (CEditUI2*)m_PaintManager.FindControl(_T("Country"));
	if (pCountryName->GetText().IsEmpty() == FALSE)
		strSubjectName.Format(_T("C=%s,"), pCountryName->GetText());

	CEditUI2* pProvince = (CEditUI2*)m_PaintManager.FindControl(_T("Province"));
	if (pProvince->GetText().IsEmpty() == FALSE)
		strSubjectName.Format(_T("S=%s,"), pProvince->GetText());

	CEditUI2* pCity = (CEditUI2*)m_PaintManager.FindControl(_T("City"));
	if (pCity->GetText().IsEmpty() == FALSE)
		strSubjectName.Format(_T("L=%s,"), pCity->GetText());

	CEditUI2* pCompany = (CEditUI2*)m_PaintManager.FindControl(_T("Company"));
	if (pCompany->GetText().IsEmpty() == FALSE)
		strSubjectName.Format(_T("O=%s,"), pCompany->GetText());

	CEditUI2* pOrgName = (CEditUI2*)m_PaintManager.FindControl(_T("OrgName"));
	if (pOrgName->GetText().IsEmpty() == FALSE)
		strSubjectName.Format(_T("OU=%s,"), pOrgName->GetText());

	CEditUI2* pEmail = (CEditUI2*)m_PaintManager.FindControl(_T("Email"));

	strSubjectName.TrimRight(_T(","));

	do 
	{
		// 获取密钥长度
		CComboUI* pEncryptLen = (CComboUI*)m_PaintManager.FindControl(_T("EncryptLen"));
		int nSelect = pEncryptLen->GetCurSel();
		DWORD dwKeyBit = (nSelect + 1) * 1024;

		CComboUI* pCertAlg = (CComboUI*)m_PaintManager.FindControl(_T("SignType"));
		SIGNATURE_ALGORITHM SignType = (SIGNATURE_ALGORITHM)pCertAlg->GetCurSel();

		CERT_REQUEST_INFO CertRequestInfo;
		CertRequestInfo.dwVersion = CERT_V1;

		CCryptHelper WinCrypt(CERT_TYPE_SIGNATURE);
		WinCrypt.SetSignatureAlgorithm(SignType);
		WinCrypt.OpenContainer(NULL, NULL);
		WinCrypt.GenericKey(dwKeyBit);
		WinCrypt.SetCertSubjectName(strSubjectName, &CertRequestInfo.Subject);

		BYTE pbData[1024*4];
		CERT_PUBLIC_KEY_INFO* pPublicKeyInfo = (CERT_PUBLIC_KEY_INFO*)pbData;
		DWORD dwLen2 = _countof(pbData);
		WinCrypt.ExportPublicKey(pPublicKeyInfo, &dwLen2);

		CertRequestInfo.SubjectPublicKeyInfo = *pPublicKeyInfo;
		
		CertRequestInfo.cAttribute = 0;
		BYTE bData[1024*2];
		DWORD dwLen = sizeof(bData);

		if (WinCrypt.CryptSignAndEncodeCertificate(&CertRequestInfo, bData, &dwLen) != FALSE)
		{
			HANDLE hFile = CreateFile(strRequestFilePath, GENERIC_ALL, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				CHAR szBase64Data[1024*2];
				DWORD dwBufferLen = _countof(szBase64Data);
				DWORD dwWriteLen = 0;
				CryptBinaryToStringA(bData, dwLen, CRYPT_STRING_BASE64REQUESTHEADER|CRYPT_STRING_NOCR, szBase64Data, &dwBufferLen);
				WriteFile(hFile, szBase64Data, dwBufferLen, &dwWriteLen, NULL);
				CloseHandle(hFile);
			}
		}
	
		MessageBox(m_hWnd, _T("证书请求文件保存成功！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
	} while (FALSE);
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
//
	CComboUI* pCertAlg = (CComboUI*)m_PaintManager.FindControl(_T("SelfCertAlg"));
	SIGNATURE_ALGORITHM SignAlg = (SIGNATURE_ALGORITHM)pCertAlg->GetCurSel();

	LPCWSTR lpszStoreName[] = {L"My", L"ROOT"};
	CComboUI* pCertSave = (CComboUI*)m_PaintManager.FindControl(_T("SelfCertSave"));
	int nStoreItem = pCertSave->GetCurSel();

	CComboUI* pSaveLocal = (CComboUI*)m_PaintManager.FindControl(_T("SelfCertSaveLocal"));
	CERT_LOCALTION CertSaveLocal = (CERT_LOCALTION)pSaveLocal->GetCurSel();
	
	CCryptHelper WinCrypt(CertType);
	WinCrypt.SetSignatureAlgorithm(SignAlg);
	WinCrypt.OpenContainer(pKeyName->GetText(), NULL);
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
	/*CDuiString strControlName = msg.pSender->GetName();
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
	}*/
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

void CSignTool::OnBtnSignCert(TNotifyUI& msg)
{
	HCRYPTPROV hCryptProv = NULL;
	do 
	{
		CEditUI2* pSignReqPath = (CEditUI2*)m_PaintManager.FindControl(_T("SignReqPath"));
		if (pSignReqPath->GetText().IsEmpty())
		{
			MessageBox(m_hWnd, _T("请选择证书请求文件后重试！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
			pSignReqPath->SetFocus();
			return;
		}

		CEditUI2* pSignCertPath = (CEditUI2*)m_PaintManager.FindControl(_T("SignCertPath"));
		if (pSignCertPath->GetText().IsEmpty())
		{
			MessageBox(m_hWnd, _T("请选择证书文件保存后重试！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
			pSignCertPath->SetFocus();
			return;
		}

		BYTE bData[1024*4];
		DWORD dwLen = _countof(bData);
		BOOL bRet = CCryptHelper::LoadCertRequestFile(pSignReqPath->GetText(), bData, &dwLen);
		if (bRet == FALSE)
			break;

		CERT_REQUEST_INFO* pRequestInfo = (CERT_REQUEST_INFO*)bData;

		CERT_INFO CryptSignInfo; 
		CryptSignInfo.dwVersion = CERT_V3;
		CryptSignInfo.Subject = pRequestInfo->Subject;
		CryptSignInfo.SubjectPublicKeyInfo = pRequestInfo->SubjectPublicKeyInfo;

		BYTE SerialNum[] = "\x01\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0F\x0F";
		CryptSignInfo.SerialNumber.cbData = 16;
		CryptSignInfo.SerialNumber.pbData = SerialNum;
		
		CERT_BASIC_CONSTRAINTS2_INFO BasicConstraintsInfo = {0};
		BasicConstraintsInfo.fCA = TRUE ;
		BasicConstraintsInfo.dwPathLenConstraint = 2;
		BasicConstraintsInfo.fPathLenConstraint = TRUE;

		BYTE bBasicInfo[1024];
		dwLen = _countof(bBasicInfo);
		CryptEncodeObjectEx(X509_ASN_ENCODING, X509_BASIC_CONSTRAINTS2, &BasicConstraintsInfo, 0, NULL, bBasicInfo, &dwLen);

		CryptSignInfo.cExtension   = 1;	
		CERT_EXTENSION CertExtension;
		CertExtension.pszObjId = szOID_BASIC_CONSTRAINTS2;
		CertExtension.fCritical = TRUE;
		CertExtension.Value.cbData = dwLen;
		CertExtension.Value.pbData = bBasicInfo;
		CryptSignInfo.rgExtension = &CertExtension;

		CryptSignInfo.IssuerUniqueId.cbData = 0 ;
		CryptSignInfo.SubjectUniqueId.cbData = 0;

		CryptSignInfo.SignatureAlgorithm.pszObjId = szOID_RSA_SHA256RSA;
		CryptSignInfo.SignatureAlgorithm.Parameters.cbData = 0;

		SYSTEMTIME SysTime;
		GetSystemTime(&SysTime);
		SystemTimeToFileTime( &SysTime , &CryptSignInfo.NotBefore);

		SysTime.wYear += 10;
		SystemTimeToFileTime( &SysTime , &CryptSignInfo.NotAfter);

		BYTE bIssuerName[100];
		DWORD dwIssuerLen = _countof(bIssuerName);
		if (CertStrToName(X509_ASN_ENCODING, _T("CN=SignTool"), CERT_X500_NAME_STR, NULL, bIssuerName, &dwIssuerLen, NULL) == FALSE)
			break;

		CryptSignInfo.Issuer.cbData = dwIssuerLen;
		CryptSignInfo.Issuer.pbData = bIssuerName;

		CRYPT_ALGORITHM_IDENTIFIER algId;
		BYTE paraData[16];
		paraData[0] = 0x05; paraData[1] = 0x00;

		algId.pszObjId = szOID_RSA_SHA256RSA;
		algId.Parameters.cbData = 0;
		algId.Parameters.pbData = paraData;

		BYTE bCertData[1024*2];
		DWORD dwCertLen = _countof(bCertData);

		CryptAcquireContext(&hCryptProv, _T("SignTool"), NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET);
		bRet = CryptSignAndEncodeCertificate(hCryptProv, AT_SIGNATURE, X509_ASN_ENCODING,  
														X509_CERT_TO_BE_SIGNED, (void*)&CryptSignInfo, &algId, NULL, bCertData, &dwCertLen);
	
		if (bRet == FALSE)
			break;

		HANDLE hFile = CreateFile(pSignCertPath->GetText(),GENERIC_ALL,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			WriteFile(hFile, bCertData, dwCertLen,&dwLen,NULL);
			CloseHandle(hFile);
		}

	} while (FALSE);

	if (hCryptProv != NULL)
		CryptReleaseContext(hCryptProv,0);
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