#include "StdAfx.h"
#include "CryptHelper.h"
#pragma comment(lib,"Crypt32.lib")

CCryptHelper::CCryptHelper(CERT_TYPE CertType)
	 : m_CertType(CertType)
	 , m_strSignatureAlgorithm(szOID_RSA_SHA256RSA)
{
	m_hCryptProv = NULL;
	m_hKey = NULL;
	m_pCertContext = NULL;
}

CCryptHelper::~CCryptHelper()
{
	if (m_hKey != NULL)
		CryptDestroyKey(m_hKey);

	if (m_hCryptProv != NULL)
		CryptReleaseContext(m_hCryptProv, 0);

	if (m_pCertContext != NULL)
		CertFreeCertificateContext(m_pCertContext);
}

BOOL CCryptHelper::GenericKey(DWORD dwKeyBit)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_hCryptProv == NULL)
			break;

		if (m_hKey != NULL)
			break;

		DWORD dwKeyLen = KEY_LENGTH_MASK &  MAKELONG(0, dwKeyBit);
		if (CryptGenKey(m_hCryptProv, m_CertType, dwKeyLen | CRYPT_EXPORTABLE , &m_hKey) == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void CCryptHelper::SetSignatureAlgorithm(SIGNATURE_ALGORITHM SignAlg)
{
	switch(SignAlg)
	{
		case RSA_SHA1RSA:
				m_strSignatureAlgorithm = szOID_RSA_SHA1RSA;
			break;
		case RSA_SHA256RSA:
				m_strSignatureAlgorithm = szOID_RSA_SHA256RSA;
			break;
		case RSA_SHA384RSA:
				m_strSignatureAlgorithm = szOID_RSA_SHA384RSA;
			break;
		case RSA_SHA512RSA:
				m_strSignatureAlgorithm = szOID_RSA_SHA512RSA;
			break;
	}
}

BOOL CCryptHelper::OpenContainer(LPCTSTR lpszContainerName, LPCTSTR lpszProviderName)
{
	BOOL bSuccess = FALSE;

	do 
	{
		if (m_hCryptProv != NULL)
			break;

		BOOL bRet = CryptAcquireContext(&m_hCryptProv, lpszContainerName, lpszProviderName, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET);
		if (bRet == FALSE)
			bRet = CryptAcquireContext(&m_hCryptProv, lpszContainerName, lpszProviderName, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET | CRYPT_NEWKEYSET);

		m_strContainer = lpszContainerName;
		m_strProviderName = lpszProviderName;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CCryptHelper::SetCertSubjectName(LPCTSTR lpszSubjectName, CERT_NAME_BLOB* pSubjectIssuerBlob)
{
	BOOL bSuccess = FALSE;
	BYTE* pbEncoded = NULL;

	do 
	{
		DWORD dwEncoded = 0;
		CertStrToName(X509_ASN_ENCODING, lpszSubjectName, CERT_X500_NAME_STR, NULL, pbEncoded, &dwEncoded, NULL);

		if (dwEncoded == 0)
			break;

		pbEncoded = (BYTE*) new BYTE[dwEncoded];
		if (pbEncoded == NULL)
			break;

		if (CertStrToName(X509_ASN_ENCODING, lpszSubjectName, CERT_X500_NAME_STR, NULL, pbEncoded, &dwEncoded, NULL) == FALSE)
			break;

		pSubjectIssuerBlob->cbData = dwEncoded;
		pSubjectIssuerBlob->pbData = pbEncoded;

		bSuccess = TRUE;
	} while (FALSE);
	
	return bSuccess;
}

BOOL CCryptHelper::ExportPublicKey(CERT_PUBLIC_KEY_INFO* pCertPublicKeyInfo, DWORD* pdwLen)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_hCryptProv == NULL)
			break;

		if (m_hKey == NULL)
			break;

		if (CryptExportPublicKeyInfo(m_hCryptProv, AT_SIGNATURE, X509_ASN_ENCODING, pCertPublicKeyInfo, pdwLen) == FALSE)
			break;
		
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CCryptHelper::CreateSelfSignedCertificate(LPCTSTR lpszSubjectName, SYSTEMTIME* pStartTime, SYSTEMTIME* pEndTime)
{
	BOOL bSuccess = FALSE;
	
	CERT_NAME_BLOB SubjectIssuerBlob;
	ZeroMemory(&SubjectIssuerBlob, sizeof(SubjectIssuerBlob));

	do 
	{
		// 释放上一次使用的句柄
		if (m_pCertContext != NULL)
		{
			CertFreeCertificateContext(m_pCertContext);
			m_pCertContext = NULL;
		}

		if (SetCertSubjectName(lpszSubjectName, &SubjectIssuerBlob) == FALSE)
			break;

		CRYPT_KEY_PROV_INFO KeyProvInfo;
		ZeroMemory(&KeyProvInfo, sizeof(CRYPT_KEY_PROV_INFO));

		CStringW strContainerName(m_strContainer);
		CStringW strProviderName(m_strProviderName);

		LPWSTR lpszContainerName = NULL;
		LPWSTR lpszProviderName = NULL;

		if (strContainerName.IsEmpty() == FALSE)
			lpszContainerName = (LPWSTR)(LPCWSTR)strContainerName;

		if (strProviderName.IsEmpty() == FALSE)
			lpszProviderName = (LPWSTR)(LPCWSTR)strProviderName;

		KeyProvInfo.pwszContainerName = lpszContainerName;
		KeyProvInfo.pwszProvName = lpszProviderName;
		KeyProvInfo.dwProvType = PROV_RSA_FULL;
		KeyProvInfo.dwFlags = CRYPT_MACHINE_KEYSET;
		KeyProvInfo.cProvParam = 0;
		KeyProvInfo.rgProvParam = NULL;
		KeyProvInfo.dwKeySpec = m_CertType;

		CRYPT_ALGORITHM_IDENTIFIER SignatureAlgorithm;
		ZeroMemory(&SignatureAlgorithm, sizeof(CRYPT_ALGORITHM_IDENTIFIER));

		// 设置签名算法
		SignatureAlgorithm.pszObjId = (LPSTR)(LPCSTR)m_strSignatureAlgorithm;

		m_pCertContext = CertCreateSelfSignCertificate(NULL, &SubjectIssuerBlob, 0, &KeyProvInfo, &SignatureAlgorithm, pStartTime, pEndTime, 0);
		if (m_pCertContext == NULL)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (SubjectIssuerBlob.cbData != 0)
		delete[] SubjectIssuerBlob.pbData;

	return bSuccess;
}

BOOL CCryptHelper::CryptSignAndEncodeCertificate(CERT_REQUEST_INFO* pRequestInfo, BYTE* pEncode, DWORD* pcbEncode)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_hCryptProv == NULL)
			break;
		if (m_hKey == NULL)
			break;

		CRYPT_ALGORITHM_IDENTIFIER CryptAlg;
		ZeroMemory(&CryptAlg, sizeof(CRYPT_ALGORITHM_IDENTIFIER));
		CryptAlg.pszObjId = (LPSTR)(LPCSTR)m_strSignatureAlgorithm;

		bSuccess = ::CryptSignAndEncodeCertificate(m_hCryptProv, m_CertType, X509_ASN_ENCODING,  
																				X509_CERT_REQUEST_TO_BE_SIGNED, (void*)pRequestInfo, 
																				&CryptAlg, NULL, pEncode, pcbEncode);
	
	} while (FALSE);

	return bSuccess;
}

BOOL CCryptHelper::LoadCertRequestFile(LPCTSTR lpszRequestFile, LPVOID lpData, DWORD* pcbLen)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	do 
	{
		hFile = CreateFile(lpszRequestFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			break;

		DWORD dwSizeHigh, dwSizeLow;
		dwSizeLow = GetFileSize(hFile, &dwSizeHigh);

		BYTE Base64Data[1024*2] = {0};
		DWORD dwLen = 0;
		ReadFile(hFile, Base64Data, dwSizeLow ,&dwLen, NULL);
	
		BYTE bBinaryData[1024*2];
		DWORD dwInfo = _countof(bBinaryData);
		if (CryptStringToBinaryA((LPCSTR)Base64Data, dwLen, CRYPT_STRING_BASE64REQUESTHEADER, bBinaryData, &dwInfo, NULL, NULL) == FALSE)
			break;

		if (CryptDecodeObjectEx(X509_ASN_ENCODING, X509_CERT_REQUEST_TO_BE_SIGNED, 
															bBinaryData, dwInfo, 0,NULL, lpData, pcbLen) == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return bSuccess;
}

BOOL CCryptHelper::AddCertificateToStore(CERT_LOCALTION CertLocal, LPCVOID lpData)
{
	BOOL bSuccess = FALSE;

	HCERTSTORE hStore = NULL;
	HCRYPTPROV_OR_NCRYPT_KEY_HANDLE hCryptProvOrNCryptkey = NULL;

	do 
	{
		if (m_pCertContext == NULL)
			break;

		DWORD dwFlag = CERT_SYSTEM_STORE_CURRENT_USER;
		if (CertLocal == CERT_LOCALTION_LOCAL_MACHINE)
			dwFlag = CERT_SYSTEM_STORE_LOCAL_MACHINE;

		// 注意：如果存在Key证书，向系统存储区写入证书可能失败
		hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, dwFlag, lpData);
		if (hStore == NULL)
			break;

		if (CertAddCertificateContextToStore(hStore, m_pCertContext, CERT_STORE_ADD_REPLACE_EXISTING,0) == FALSE)
			break;

		DWORD dwKeySpec;
		BOOL fCallerFreeProvOrNCryptKey = FALSE;
		if (CryptAcquireCertificatePrivateKey(m_pCertContext, 0, NULL, &hCryptProvOrNCryptkey, &dwKeySpec, &fCallerFreeProvOrNCryptKey) == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (hStore != NULL)
		CertCloseStore(hStore, 0);
	
	if (hCryptProvOrNCryptkey != NULL)
		CryptReleaseContext(hCryptProvOrNCryptkey, 0);

	return bSuccess;
}