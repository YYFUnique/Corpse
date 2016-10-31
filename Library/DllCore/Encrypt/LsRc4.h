#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CLsRc4
{
public:
	void Init(const BYTE* lpKey , DWORD dwKeyLen);

	//加密与解密使用相同函数
	void Crypt(const BYTE* lpInput , BYTE* lpOutput , DWORD dwDataLen);

	static BOOL CryptFile(LPCTSTR lpszInputFile , DWORD dwHeadLen ,LPCTSTR lpszOutputFile , const void* lpszFilePwd , DWORD dwPwdLen);

protected:
	BYTE m_Data[256];
};

