#pragma once
#include "Hash.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
    uint32 total[2];
    uint32 state[8];
    uint8 buffer[64];
}SHA256_CTX;

class LS_COMMON_API CSHA256: public CHash 
{
private:
	SHA256_CTX context;
public:
//==================================================================
//函 数 名: Init
//功    能: Hash算法初始操作(尚无实际功能,仅为实现底层接口)
//输入参数: 无输入参数
//返 回 值: 无返回值
//==================================================================
	void Init();

//==================================================================
//函 数 名: Update
//功    能: 输入加密数据(尚无实际功能,仅为实现底层接口)
//输入参数: [in] pBuffer, 加密数据缓存地址
//			[in] dwSize, 加密数据长度
//返 回 值: 无返回值
//==================================================================
	void Update(BYTE* pBuffer, DWORD dwSize);

//==================================================================
//函 数 名: Final
//功    能: 获得加密后的HASH值(尚无实际功能,仅为实现底层接口)
//输入参数: [out] pHashvalue, Hash值
//返 回 值: 返回HASH值的位数
//==================================================================
	DWORD Final(BYTE* pHashvalue);

private:
//==================================================================
//函 数 名: sha256_starts
//功    能: 初始化SHA256加密算法
//输入参数: [out] ctx, SHA256算法上下文
//返 回 值: 无返回值
//==================================================================
	void sha256_starts( SHA256_CTX *ctx );

//==================================================================
//函 数 名: sha256_update
//功    能: 输入待加密结果
//输入参数: [in] ctx, 算法上下文
//			[in] input, 待加密数据指针
//			[in] length, 待加密数据长度
//返 回 值: 无返回值
//==================================================================
    void sha256_update( SHA256_CTX *ctx, const uint8 *input, uint32 length );

//==================================================================
//函 数 名: sha256_finish
//功    能: 获取加密后密文
//输入参数: [in]  ctx, 加密算法上下文
//			[out] digest,加密后明文
//返 回 值: 无返回值
//==================================================================
    void sha256_finish( SHA256_CTX *ctx, uint8 digest[32] );

};

