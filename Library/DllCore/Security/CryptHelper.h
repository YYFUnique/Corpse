#pragma once
#include <WinCrypt.h>

typedef enum tagCERT_TYPE
{
	CERT_TYPE_ENCRYPT		= AT_KEYEXCHANGE,
	CERT_TYPE_SIGNATURE	= AT_SIGNATURE,
}CERT_TYPE;

typedef enum tagSIGNATURE_ALGORITHM
{
		RSA_SHA1RSA			= 0,
		RSA_SHA256RSA		= 1,
		RSA_SHA384RSA    = 2,
		RSA_SHA512RSA    = 3,
}SIGNATURE_ALGORITHM;

typedef enum tagCERT_LOCALTION
{
	CERT_LOCALTION_LOCAL_USER			= 0,
	CERT_LOCALTION_LOCAL_MACHINE		= 1,
}CERT_LOCALTION;

class DLL_API CCryptHelper
{
public:
	CCryptHelper(CERT_TYPE CertType);
	~CCryptHelper();

public:

	/************************************************************************/
	/* 函数名称：GenericKey							                                                   */
	/* 函数功能：随机产生公私钥密钥对															   */
	/* 函数参数：dwKeyBit 密钥长度(bit)														   */
	/*	返 回  值：返回TRUE，表示成功，返回FALSE，表示失败						   */
	/************************************************************************/
	BOOL GenericKey(DWORD dwKeyBit);

	void SetSignatureAlgorithm(SIGNATURE_ALGORITHM SignAlg);

	BOOL OpenContainer(LPCTSTR lpszContainerName, LPCTSTR lpszProviderName);

	/************************************************************************/
	/* 函数名称：CreateSelfSignedCertificate                                                  */
	/* 函数功能：创建自签名证书																	   */
	/* 函数参数：lpszSubjectName 使用者信息												   */
	/*                  CN=证书通用名称/域名, C：使用者所在国家，L：城市  		   */
	/*					 S：使用者所在省份, O:使用者所在组织,OU：使用者所在部门  */
	/*					E/Email:使用者电子邮件														   */
	/************************************************************************/
	BOOL CreateSelfSignedCertificate(LPCTSTR lpszSubjectName, SYSTEMTIME* pStartTime, SYSTEMTIME* pEndTime);

	/************************************************************************/
	/* 函数名称：AddCertificateToStore		                                                   */
	/* 函数参数：lpszSubjectName 使用者信息												   */
	/************************************************************************/
	BOOL AddCertificateToStore(CERT_LOCALTION CertLocal, LPCVOID lpData);

	BOOL SetCertSubjectName(LPCTSTR lpszSubjectName, CERT_NAME_BLOB* pSubjectIssuerBlob);

	BOOL ExportPublicKey(CERT_PUBLIC_KEY_INFO* pCertPublicKeyInfo, DWORD* pdwLen);

	/************************************************************************/
	/* 函数名称：CryptSignAndEncodeCertificate                                           */
	/* 函数参数：使用私钥签名证书请求文件													   */
	/************************************************************************/
	BOOL CryptSignAndEncodeCertificate(CERT_REQUEST_INFO* pRequestInfo, BYTE* pEncode, DWORD* pcbEncode);

	static BOOL LoadCertRequestFile(LPCTSTR lpszRequestFile, LPVOID lpData, DWORD* pcbLen);
protected:
	CERT_TYPE		m_CertType;
	HCRYPTKEY		m_hKey;								// 证书存储区句柄
	HCRYPTPROV m_hCryptProv;						//
	CString				m_strContainer;					// 密钥容器
	CString				m_strProviderName;			// 提供者
	CStringA			m_strSignatureAlgorithm;	// 密钥算法
	PCCERT_CONTEXT	m_pCertContext;			// 证书句柄
};