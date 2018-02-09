#include "StdAfx.h"
#include "Encrypt.h"
#include <atlstr.h>
#include <WinCrypt.h>
#pragma comment(lib,"crypt32.lib")

BOOL CreateSelfSignedCertificate(LPCTSTR lpszContainerName)
{
	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY		hKey = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	BYTE* pbEncoded = NULL;
	HCERTSTORE hStore = NULL;
	HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptkey = NULL;

	do 
	{
		//获取指定服务提供者
		BOOL bRet = CryptAcquireContext(&hCryptProv, lpszContainerName, NULL, PROV_RSA_FULL,CRYPT_MACHINE_KEYSET);
		if (bRet == FALSE) 
			//如果指定服务提供者不存在，新建一个
			bRet = CryptAcquireContext(&hCryptProv, lpszContainerName, NULL, PROV_RSA_FULL,CRYPT_MACHINE_KEYSET|CRYPT_NEWKEYSET);

		if (bRet == FALSE)
			break;

		if (CryptGenKey(hCryptProv, AT_SIGNATURE, 0x08000000, &hKey) == FALSE)
			break;
		

		LPCTSTR lpszX500 = _T("CN=Lonsinassss,T=Success");
		DWORD dwEncoded = 0;
		CertStrToName(X509_ASN_ENCODING, lpszX500, CERT_X500_NAME_STR, NULL, pbEncoded, &dwEncoded, NULL);
		
		if (dwEncoded == 0)
			break;

		pbEncoded = (BYTE*) new BYTE[dwEncoded];
		if (pbEncoded == NULL)
			break;
		if (CertStrToName(X509_ASN_ENCODING, lpszX500, CERT_X500_NAME_STR, NULL, pbEncoded, &dwEncoded, NULL) == FALSE)
			break;
		
		CERT_NAME_BLOB SubjectIssuerBlob;
		ZeroMemory(&SubjectIssuerBlob, sizeof(CERT_NAME_BLOB));
		SubjectIssuerBlob.cbData = dwEncoded;
		SubjectIssuerBlob.pbData = pbEncoded;

		CRYPT_KEY_PROV_INFO KeyProvInfo;
		ZeroMemory(&KeyProvInfo, sizeof(CRYPT_KEY_PROV_INFO));

		CStringW strContainerName(lpszContainerName);

		KeyProvInfo.pwszContainerName = (LPWSTR)(LPCWSTR)strContainerName;
		KeyProvInfo.pwszProvName = NULL;
		KeyProvInfo.dwProvType = PROV_RSA_FULL;
		KeyProvInfo.dwFlags = CRYPT_MACHINE_KEYSET;
		KeyProvInfo.cProvParam = 0;
		KeyProvInfo.rgProvParam = NULL;
		KeyProvInfo.dwKeySpec = AT_SIGNATURE;

		CRYPT_ALGORITHM_IDENTIFIER SignatureAlgorithm;
		ZeroMemory(&SignatureAlgorithm, sizeof(CRYPT_ALGORITHM_IDENTIFIER));

		SignatureAlgorithm.pszObjId = szOID_RSA_SHA1RSA;
		SYSTEMTIME EndTime;
		GetSystemTime(&EndTime);
		EndTime.wYear += 5;

		pCertContext = CertCreateSelfSignCertificate(NULL, &SubjectIssuerBlob, 0, &KeyProvInfo, &SignatureAlgorithm, 0, &EndTime, 0);
		if (pCertContext == NULL)
			break;

		hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, L"ROOT");
		if (hStore == NULL)
			break;

		if (CertAddCertificateContextToStore(hStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING,0) == FALSE)
			break;

		DWORD dwKeySpec;
		BOOL fCallerFreeProvOrNCryptKey = FALSE;
		if (CryptAcquireCertificatePrivateKey(pCertContext, 0, NULL, &hCryptProvOrNCryptkey, &dwKeySpec, &fCallerFreeProvOrNCryptKey) == FALSE)
			break;

	} while (FALSE);

	if (hKey != NULL)
		CryptDestroyKey(hKey);
	if (hCryptProv != NULL)
		CryptReleaseContext(hCryptProv,0);
	if (pbEncoded != NULL)
	{
		delete[] pbEncoded;
		pbEncoded = NULL;
	}
	if (hCryptProvOrNCryptkey != NULL)
	{
		CryptReleaseContext(hCryptProvOrNCryptkey, 0);
	}
	if (pCertContext != NULL)
	{
		CertFreeCertificateContext(pCertContext);
	}
	if (hStore != NULL)
	{
		CertCloseStore(hStore, 0);
	}

	return TRUE;
}