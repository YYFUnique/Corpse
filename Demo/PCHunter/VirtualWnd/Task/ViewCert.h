#pragma once

typedef struct tagUSBKEY_CERT_INFO
{
	CDuiString strCertName;
	CDuiString strIssuerName;
	FILETIME	   FtNotBefore;
	FILETIME	   FtNotAfter;
	CDuiString strKeyContainer;
	CDuiString strProviderName;
	DWORD	  dwKeySpec;
	DWORD	  dwKeyLen;
	PCCERT_CONTEXT pCertContext;
}USBKEY_CERT_INFO,*PUSBKEY_CERT_INFO;

typedef CDuiList<USBKEY_CERT_INFO,USBKEY_CERT_INFO&> CCertInfoList;

class CViewCert : public CNotifyPump
{
public:
	CViewCert();
	~CViewCert();

public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnLoadItem(TNotifyUI& msg);
	void OnRefresh(TNotifyUI& msg);
	void OnItemActive(TNotifyUI& msg);
	BOOL EnumAllCertInfo(CCertInfoList& CertInfoList);
protected:
	BOOL					   m_bLoad;
	CPaintManagerUI* m_pPaintManager;
};