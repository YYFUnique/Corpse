/********************************************************************************************
 * Project:		Wincrypto																	*
 * Version:		1.00.00																		*
 * Platform:	XP/Vista/Windows 7/Windows 8.1												*
 *																							*
 * Modul:		CryptoApi Crypto-API														*
 * File:		CryptoApi.cpp																*
 *																							*
 * 16.06.2014	initial version																*	
 *******************************************************************************************/
#include "StdAfx.h"
#include "CryptoApi.h"

//#include <wincrypt.h>
//#pragma comment (lib, "advapi32")
//
//CryptoApi::CryptoApi() : m_hCryptoProvider(0), m_hHashPassword(0), m_hCryptKey(0), m_rc(0)
//{
//}
//
//CryptoApi::~CryptoApi()
//{
//	if( m_hCryptKey )
//	{
//		::CryptDestroyKey( m_hCryptKey );
//		m_hCryptKey = 0;
//	}
//
//	if( m_hHashPassword )
//	{
//		::CryptDestroyHash( m_hHashPassword ); 
//		m_hHashPassword = 0;
//	}
//	if( m_hCryptoProvider )
//	{
//		::CryptReleaseContext( m_hCryptoProvider, 0);
//		m_hCryptoProvider = 0;
//	}
//}
//
//int CryptoApi::Init(LPCTSTR password)
//{
//	if( m_hCryptoProvider != 0 )
//	{
//		ASSERT( (m_hHashPassword != 0) && (m_hCryptKey != 0) );
//		return 0;
//	}
//	//if( !::CryptAcquireContext( &m_hCryptoProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
//	if( ::CryptAcquireContext( &m_hCryptoProvider, NULL, NULL/*Default*/, PROV_RSA_AES, CRYPT_VERIFYCONTEXT ) )
//	{
//		//Hash Password
//		// CALG_SHA1 OK
//		// CALG_AES_128 / CALG_AES_256 => error
//		if( ::CryptCreateHash( m_hCryptoProvider, CALG_SHA1, 0, 0, &m_hHashPassword ) )
//		{
//			// Hash for the password. 
//			if( ::CryptHashData( m_hHashPassword, (BYTE *)password, (DWORD) _tcslen(password) * sizeof(TCHAR), 0 ) )
//			{
//				// Session key from the hash
//				if( ::CryptDeriveKey( m_hCryptoProvider, CALG_AES_256, m_hHashPassword, CRYPT_CREATE_SALT, &m_hCryptKey ) )
//				{
//					TRACE( TEXT("Crypto-API OK\n") ); 
//					return ERROR_SUCCESS;
//				}
//				else
//				{
//					TRACE( TEXT("Error in CryptDeriveKey\n") ); 
//				}
//			}
//			else
//			{
//				TRACE( TEXT("Error in CryptHashData\n") ); 
//			}
//		}
//		else
//		{ 
//			TRACE( TEXT("Error in CryptCreateHash\n") );
//		}
//	}
//	else
//	{
//        TRACE( TEXT("Error in CryptAcquireContext\n") );
//    }
//
//	return HandleError();
//}
//
//DWORD CryptoApi::HandleError()
//{
//	m_rc = GetLastError();
//    TRACE( TEXT("Crypto Errortext = %ld\n"), m_rc ); 
//      
//	return m_rc;
//}
//BOOL CryptoApi::EnCrypt( BYTE* pData, DWORD &lenBuffer)
//{
//	DWORD len = (DWORD) _tcslen( (TCHAR*) pData) * sizeof(TCHAR);
//
//	BOOL rc = EnCrypt( pData, len, lenBuffer );
//
//	lenBuffer = len;//
//
//	return rc;
//}
////
//BOOL CryptoApi::EnCrypt( BYTE* pData, DWORD &lenData, DWORD lenBuffer)
//{
//	ASSERT( (m_hCryptoProvider != 0) && (m_hHashPassword != 0) && (m_hCryptKey != 0) );
//	BOOL b = ::CryptEncrypt( m_hCryptKey, NULL, TRUE, 0,  pData, &lenData, lenBuffer );
//
//    if( !b)
//	{ 
//        TRACE( TEXT("Error during CryptEncrypt\n")); 
//		HandleError();
//    } 
//
//	return b;
//}
//BOOL CryptoApi::DeCrypt( BYTE* pData, DWORD &len)
//{
//	ASSERT( (m_hCryptoProvider != 0) && (m_hHashPassword != 0) && (m_hCryptKey != 0) );
//	BOOL b = ::CryptDecrypt( m_hCryptKey, NULL, TRUE, 0,  pData, &len );
//
//    if( !b)
//	{ 
//        TRACE( TEXT("Error during DeCrypt\n") ); 
//		HandleError();
//    } 
//
//	return b;
//}
//
//char* CryptoApi::NewCharsFromBytes( const BYTE *pBytes, int cnt)
//{
//	char *pChars = new char[ 2 * (cnt + 1)];
//
//	for( int i = 0; i < cnt; i++ )
//	{
//		sprintf_s( &pChars[2*i], 2, "%02x", pBytes[i] );
//	}
//
//	return pChars;
//}
