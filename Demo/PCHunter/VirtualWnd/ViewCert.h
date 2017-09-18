#pragma once
#include <Cryptuiapi.h>

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

class CViewCert : public CBase
{
public:
	CViewCert();
	~CViewCert();

public:
	void Notify(TNotifyUI& msg);
	void OnPaint();
protected:
	void OnItemDbClick(TNotifyUI& msg);
	BOOL EnumAllCertInfo(CCertInfoList& CertInfoList);
};