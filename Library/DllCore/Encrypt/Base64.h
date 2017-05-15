#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define ATL_BASE64_FLAG_NONE	0
#define ATL_BASE64_FLAG_NOPAD	1
#define ATL_BASE64_FLAG_NOCRLF  2

//==================================================================
//�� �� ��: Base64Encode
//��    ��: Base64�㷨����
//�������: [in]  pbSrcData, �������ݻ����ַ
//			[in]  nSrcLen, �������ݻ��泤��
//			[out] szDest, ���ܺ����ݻ����ַ
//			[out] pnDestLen, ���ܺ����ݻ��泤��
//			[in]  dwFlags, ������ʽ
//					ATL_BASE64_FLAG_NONE ���������Կո���»��߷ָ�(Ĭ��)
//					ATL_BASE64_FLAG_NOPAD �����������»��߷ָ�
//					ATL_BASE64_FLAG_NOCRLF ���������Կո�ָ� 
//�� �� ֵ: �������óɹ�����TRUE,���򷵻�FALSE
//==================================================================
DLL_API BOOL Base64Encode(LPCTSTR lpszSrcData,int nSrcLen,LPTSTR szDest,int *pnDestLen,DWORD dwFlags=ATL_BASE64_FLAG_NONE);

//DLL_API BOOL URLEncode(LPCWSTR lpwURL, CString& strCryptURL);

//DLL_API BOOL URLDecode(LPCTSTR lpszURL, LPTSTR lpszCrypt, DWORD& dwSize);