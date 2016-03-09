#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

/*****************************************************************************
 * 这几个函数为本地安全检测工具的数据文件加密解密所用
 */

//==================================================================
//函 数 名: LibEncryptFile
//功    能: 创建目的文件的加密备份
//输入参数: [in]  lpszSrcFile, 源文件路径
//			[out] lpszDestFile, 源文件的加密备份文件路径
//返 回 值: 文件加密备份成功返回TRUE,否则返回FALSE
//==================================================================
LS_COMMON_API BOOL LibEncryptFile(LPCTSTR lpszSrcFile, LPCTSTR lpszDestFile);

//==================================================================
//函 数 名: LibDecrypt
//功    能: 目的文件解密
//输入参数: [in] lpszEncFile,目的文件位置
//返 回 值: 返回解密后文件数据缓存首地址
//==================================================================
LS_COMMON_API LPSTR LibDecrypt(LPCTSTR lpszEncFile);

//==================================================================
//函 数 名: LibBufferFree
//功    能: 释放解密文件数据缓存
//输入参数: [in,out] 解密文件数据缓存首地址
//返 回 值: 无返回值
//==================================================================
LS_COMMON_API void LibBufferFree(void *pBuffer);
