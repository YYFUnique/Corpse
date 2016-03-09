#pragma once
#include "Hash.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

typedef struct 
{
    unsigned long state[5];
    unsigned long count[2];
    unsigned char buffer[64];
} SHA1_CTX;

class LS_COMMON_API CSHA1: public CHash
{
public:
	CSHA1();
private:
	SHA1_CTX context;
public:
	void Init();
	void Update(BYTE* pBuffer, DWORD dwSize);
	DWORD Final(BYTE* pHashvalue);
private:

//==================================================================
//函 数 名: SHA1Init
//功    能: 初始化SHA1算法
//输入参数: 无输入参数
//返 回 值: 无返回值
//==================================================================
	void SHA1Init();

//==================================================================
//函 数 名: SHA1Update
//功    能: SHA1算法加密
//输入参数: [in] 加密数据缓存地址
//			[in] 加密数据长度
//返 回 值: 无返回值
//==================================================================
	void SHA1Update(const void* input,DWORD inputLen);

//==================================================================
//函 数 名: SHA1Final
//功    能: 获取经SHA1加密后HASH值
//输入参数: [out] HASH值
//返 回 值: 返回HASH值长度(20)
//==================================================================
	DWORD SHA1Final(BYTE digest[20]);

//==================================================================
//函 数 名: SHA1Transform
//功    能: SHA1数组移位加密算法
//输入参数: [in,out] state, 输入待加密数组,输出移位加密后数组
//			[in]	 buffer, 加密用数据块
//返 回 值: 无返回值
//==================================================================
	void SHA1Transform(unsigned long state[5], unsigned char buffer[64]);	

//==================================================================
//函 数 名: GetHashAlgName
//功    能: 获取HASH算法名称("SHA1")
//输入参数: 无输入参数
//返 回 值: HASH算法名称
//==================================================================
	const TCHAR* GetHashAlgName();

//==================================================================
//函 数 名: GetHashLen
//功    能: 获取HASH值长度(20)
//输入参数: 无输入参数
//返 回 值: HASH值长度
//==================================================================
	DWORD GetHashLen();

};

