#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//==================================================================
//�� �� ��: EncryptString
//��    ��: �����ַ���
//�������: [in]  lpszText, �������ַ���
//			[out] lpbyEncryptText, ���ܺ��ַ���
//�� �� ֵ: ���ؼ��ܺ����ĳ���
//		ע: ����������ַ����ʹ������ܺ�����ָ��ͬһ��ַ
//==================================================================
LS_COMMON_API DWORD EncryptString(LPCTSTR lpszText,BYTE* lpbyEncryptText);
//==================================================================
//�� �� ��: DecryptString
//��    ��: �����ַ���
//�������: [in]  lpbyEncryptText, ��������
//			[in]  dwEncryptBufferLen, �������ĳ���
//			[out] lpszText, ������ܺ�����
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMON_API void DecryptString(BYTE* lpbyEncryptText,DWORD dwEncryptBufferLen,LPTSTR lpszText);
