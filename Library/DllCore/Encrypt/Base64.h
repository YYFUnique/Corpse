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
//函 数 名: Base64Encode
//功    能: Base64算法加密
//输入参数: [in]  pbSrcData, 加密数据缓存地址
//			[in]  nSrcLen, 加密数据缓存长度
//			[out] szDest, 加密后数据缓存地址
//			[out] pnDestLen, 加密后数据缓存长度
//			[in]  dwFlags, 加密形式
//					ATL_BASE64_FLAG_NONE 加密数据以空格和下划线分割(默认)
//					ATL_BASE64_FLAG_NOPAD 加密数据以下划线分割
//					ATL_BASE64_FLAG_NOCRLF 加密数据以空格分割 
//返 回 值: 函数调用成功返回TRUE,否则返回FALSE
//==================================================================
DLL_API BOOL Base64Encode(const BYTE *pbSrcData,int nSrcLen,LPSTR szDest,int *pnDestLen,DWORD dwFlags=ATL_BASE64_FLAG_NONE);
