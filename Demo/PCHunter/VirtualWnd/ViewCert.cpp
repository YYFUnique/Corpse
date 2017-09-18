#include "StdAfx.h"

#include <atltime.h>
#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"Cryptui.lib")

CViewCert::CViewCert()
{

}

CViewCert::~CViewCert()
{

}

void CViewCert::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_ITEMACTIVATE)
		OnItemDbClick(msg);
	else if (msg.sType == DUI_MSGTYPE_KILLFOCUS)
		OutputDebugString(msg.pSender->GetName());
	else
		CBase::Notify(msg);
}

void CViewCert::OnItemDbClick(TNotifyUI& msg)
{
	//证书列表对话框
	//CRYPTUI_CERT_MGR_STRUCT CryptCertMgr;
	//ZeroMemory(&CryptCertMgr,sizeof(CryptCertMgr));
	//CryptCertMgr.dwSize = sizeof(CryptCertMgr);
	//CryptCertMgr.dwFlags = 0;
	//CryptCertMgr.hwndParent = m_pPaintManager->GetPaintWindow();
	//CListTextElementUI* pTextElement = (CListTextElementUI*)msg.pSender;
	//CryptCertMgr.pwszTitle = pTextElement->GetText(0);
	////CryptCertMgr.pszInitUsageOID = 
	//CryptUIDlgCertMgr(&CryptCertMgr);

	CERT_CONTEXT* pCertContext = NULL;
	CRYPTUI_VIEWCERTIFICATE_STRUCT CryptSelectCert;
	ZeroMemory(&CryptSelectCert,sizeof(CryptSelectCert));
	CryptSelectCert.dwSize = sizeof(CryptSelectCert);
	CryptSelectCert.dwFlags = CRYPTUI_DISABLE_ADDTOSTORE;
	
	CListTextElementUI* pTextElement = (CListTextElementUI*)msg.pSender;
	CryptSelectCert.szTitle = pTextElement->GetText(0);

	CryptSelectCert.pCertContext = (PCCERT_CONTEXT)pTextElement->GetTag();
	CryptSelectCert.hwndParent = m_pPaintManager->GetPaintWindow();

	BOOL bPropertiesChanged = FALSE;
	CryptUIDlgViewCertificate(&CryptSelectCert,&bPropertiesChanged);

	//ReleaseCapture();
	//避免主界面没有收到WM_LBUTTONUP导致未能正常释放鼠标键盘Capture
	//并且在这里手动释放Capture无效，故采用以下方式
	PostMessage(m_pPaintManager->GetPaintWindow(),WM_LBUTTONUP,msg.wParam,msg.lParam);
}

void CViewCert::OnPaint()
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Cert"));
	if (pList == NULL)
		return;

	if (pList->GetCount())
		pList->RemoveAll();

	CCertInfoList CertInfoList;
	BOOL bRet = EnumAllCertInfo(CertInfoList);

	POSITION pos = CertInfoList.GetHeadPosition();
	int nIndex = 0;
	while(pos)
	{
		const USBKEY_CERT_INFO& UsbkeyCertInfo = CertInfoList.GetNext(pos);
		CListTextElementUI* pTextElement = new CListTextElementUI;
		pList->Add(pTextElement);

		//证书名称
		pTextElement->SetText(0,UsbkeyCertInfo.strCertName);
		//证书颁发者
		pTextElement->SetText(1,UsbkeyCertInfo.strIssuerName);
		CDuiString strNotAfter;
		CTime TimeNotAfter(UsbkeyCertInfo.FtNotAfter);
		strNotAfter.Format(_T("%d-%02d-%02d"),TimeNotAfter.GetYear(),TimeNotAfter.GetMonth(),TimeNotAfter.GetDay());
		//截止日期
		pTextElement->SetText(2,strNotAfter);
		//密钥集
		pTextElement->SetText(3,UsbkeyCertInfo.strKeyContainer);
		//服务提供者
		pTextElement->SetText(4,UsbkeyCertInfo.strProviderName);
		//密钥类型
		LPCTSTR lpszKeyType = _T("加密密钥");
		if (UsbkeyCertInfo.dwKeySpec == AT_SIGNATURE)
			lpszKeyType = _T("签名密钥");
		pTextElement->SetText(5,lpszKeyType);
		//密钥长度
		TCHAR szKeyLen[20];
		_stprintf_s(szKeyLen, _countof(szKeyLen), _T("%u"), UsbkeyCertInfo.dwKeyLen);
		pTextElement->SetText(6, szKeyLen);

		pTextElement->SetTag((UINT_PTR)UsbkeyCertInfo.pCertContext);
	}
}

BOOL CViewCert::EnumAllCertInfo(CCertInfoList& CertInfoList)
{
	BOOL bSuccess = FALSE;
	HCERTSTORE hCertStore = NULL;
	PCCERT_CONTEXT pCertContext = NULL;

	do 
	{
		//RunFunctionAsCurrentUser();
		hCertStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,NULL,
				CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG,L"MY");

		if (hCertStore==NULL)
			break;

		while (pCertContext = CertEnumCertificatesInStore(hCertStore, pCertContext))
		{	
			USBKEY_CERT_INFO UsbKeyCertInfo;
			TCHAR szCertName[MAX_PATH],szIssuerName[MAX_PATH];
			if (CertGetNameString(pCertContext,CERT_NAME_SIMPLE_DISPLAY_TYPE,0,NULL, szCertName,_countof(szCertName)) != 0)
				UsbKeyCertInfo.strCertName = szCertName;

			if (CertGetNameString(pCertContext,CERT_NAME_SIMPLE_DISPLAY_TYPE,CERT_NAME_ISSUER_FLAG,NULL,szIssuerName,_countof(szIssuerName)) != 0)
				UsbKeyCertInfo.strIssuerName = szIssuerName;

			TCHAR szCertInfoBuffer[8192];
			CRYPT_KEY_PROV_INFO* pCryptKeyProvInfo=(CRYPT_KEY_PROV_INFO*)szCertInfoBuffer;

			DWORD dwBufLen=sizeof(szCertInfoBuffer);
			if (CertGetCertificateContextProperty(pCertContext,CERT_KEY_PROV_INFO_PROP_ID ,szCertInfoBuffer,&dwBufLen)==FALSE)
				continue;

			UsbKeyCertInfo.pCertContext = CertDuplicateCertificateContext(pCertContext);

			UsbKeyCertInfo.dwKeySpec = pCryptKeyProvInfo->dwKeySpec;
			UsbKeyCertInfo.strKeyContainer = pCryptKeyProvInfo->pwszContainerName;
			UsbKeyCertInfo.strProviderName = pCryptKeyProvInfo->pwszProvName;

			UsbKeyCertInfo.FtNotAfter = pCertContext->pCertInfo->NotAfter;
			UsbKeyCertInfo.FtNotBefore = pCertContext->pCertInfo->NotBefore;
			UsbKeyCertInfo.dwKeyLen = CertGetPublicKeyLength(X509_ASN_ENCODING,&pCertContext->pCertInfo->SubjectPublicKeyInfo);
			CertInfoList.AddTail(UsbKeyCertInfo);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (pCertContext != NULL)
		CertFreeCertificateContext(pCertContext);

	if (hCertStore != NULL)
		CertCloseStore(hCertStore,0);

	return bSuccess;
}