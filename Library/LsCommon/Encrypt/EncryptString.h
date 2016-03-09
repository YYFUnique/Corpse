#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

//==================================================================
//函 数 名: EncryptString
//功    能: 加密字符串
//输入参数: [in]  lpszText, 待加密字符串
//			[out] lpbyEncryptText, 加密后字符串
//返 回 值: 返回加密后密文长度
//		注: 允许传入加密字符串和传出加密后密文指向同一地址
//==================================================================
LS_COMMON_API DWORD EncryptString(LPCTSTR lpszText,BYTE* lpbyEncryptText);
//==================================================================
//函 数 名: DecryptString
//功    能: 解密字符串
//输入参数: [in]  lpbyEncryptText, 输入密文
//			[in]  dwEncryptBufferLen, 输入密文长度
//			[out] lpszText, 输出解密后明文
//返 回 值: 无返回值
//==================================================================
LS_COMMON_API void DecryptString(BYTE* lpbyEncryptText,DWORD dwEncryptBufferLen,LPTSTR lpszText);
