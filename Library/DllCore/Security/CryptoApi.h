#pragma once
//
//#include <wincrypt.h>
//
//#ifdef LS_STATIC_LIB_CALL
//#define DLL_API 
//#elif defined  DLL_EXPORTS
//#define DLL_API __declspec(dllexport)
//#else
//#define DLL_API __declspec(dllimport)
//#endif
//
//class DLL_API CryptoApi
//{
//public:
//	CryptoApi();
//	~CryptoApi();
//
//	static char* NewCharsFromBytes(const BYTE *pBytes, int cnt);
//
//	int Init(LPCTSTR password);
//	BOOL EnCrypt( BYTE* pData, DWORD &lenBuffer);
//	BOOL EnCrypt( BYTE* pData, DWORD &lenData, DWORD lenBuffer);
//	int DeCrypt( BYTE* pData, DWORD &len);
//
//protected:
//	HCRYPTPROV	m_hCryptoProvider;
//	HCRYPTHASH	m_hHashPassword;
//	HCRYPTKEY	m_hCryptKey;
//	DWORD		m_rc;
//
//	DWORD HandleError();
//};
