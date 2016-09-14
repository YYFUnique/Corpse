#pragma once

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif

class YCLIB_CORE_API CLsRc4
{
public:
	void Init(const BYTE* lpKey , DWORD dwKeyLen);

	//加密与解密使用相同函数
	void Crypt(const BYTE* lpInput , BYTE* lpOutput , DWORD dwDataLen);

	static BOOL CryptFile(LPCTSTR lpszInputFile , DWORD dwHeadLen ,LPCTSTR lpszOutputFile , const void* lpszFilePwd , DWORD dwPwdLen);

protected:
	BYTE m_Data[256];
};

