#include "Hash.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#ifndef MD5_H
#define MD5_H

typedef struct 
{
	DWORD state[4];        /* state (ABCD) */
	DWORD count[2];        /* number of bits, modulo 2^64 (lsb first) */
	BYTE buffer[64];       /* input buffer */
} MD5_CTX;

/* MD5 Class. */
class DLL_API CMD5: public CHash 
{
public:
	CMD5();
private:
	MD5_CTX context;
public:

//==================================================================
//函 数 名: Init
//功    能: 初始化MD5算法
//输入参数: 无输入参数
//返 回 值: 无返回值
//==================================================================
	void Init();

//==================================================================
//函 数 名: Update
//功    能: 输入加密数据
//输入参数: [in] pBuffer, 加密数据缓存地址
//			[in] dwSize, 加密数据长度
//返 回 值: 无返回值
//==================================================================
	void Update(BYTE* pBuffer, DWORD dwSize);

//==================================================================
//功    能: 获得加密后的HASH值
//输入参数: [out] pHashvalue, Hash值
//返 回 值: 返回HASH值的位数
//==================================================================
	DWORD Final(BYTE* pHashvalue); 

private:

//==================================================================
//函 数 名: MD5Init
//功    能: 初始化MD5算法
//输入参数: [out] MD5算法上下文
//返 回 值: 无返回值
//==================================================================
	void MD5Init (MD5_CTX *context);

//==================================================================
//函 数 名: MD5Update
//功    能: 输入加密数据
//输入参数: [in] context, MD5加密上下文
//			[in] input, 加密数据缓存地址
//			[in] inputLen,加密数据缓存长度
//返 回 值: 无返回值
//==================================================================
	void MD5Update (MD5_CTX *context, void* input,DWORD inputLen);

//==================================================================
//函 数 名: MD5Final
//功    能: 获取加密生成的HASH值
//输入参数: [in]  context, MD5加密上下文
//		    [out] digest, 经MD5算法生成HASH值
//返 回 值: 无返回值
//==================================================================
	void MD5Final (MD5_CTX* context, BYTE digest[16]);

//==================================================================
//函 数 名: MD5Transform
//功    能: MD5数组数据移位加密算法
//输入参数: [in,out] state, 传入数组,传出移位加密后数组
//			[in]     block, 加密用数据块
//返 回 值: 无返回值
//==================================================================
	void MD5Transform(DWORD state[4], BYTE block[64]);

//==================================================================
//函 数 名: Decode
//功    能: 解密
//输入参数: [out] output, 解密后数据缓存地址
//			[in]  input, 传入解要解密数据缓存地址
//			[in]  len, 解密数据(解密后数据)长度,长度为4的整数倍
//返 回 值: 无返回值
//==================================================================
    void Decode(DWORD* output,BYTE* input, DWORD len);

//==================================================================
//函 数 名: Encode
//功    能: 加密
//输入参数: [out] output,加密后数据内容缓存地址 
//			[in]  input, 传入要加密数据缓存地址
//			[in]  len, 加密数据(加密后数据)长度,长度为4的整数倍
//返 回 值: 无返回值
//==================================================================
    void Encode(BYTE* output,DWORD* input, DWORD len);

public:

//==================================================================
//函 数 名: IsCorrectOfMd5
//功    能: 判定文件的MD5是否正确
//输入参数: LPCTSTR lpcszFileName,传入文件名
//			LPCTSTR lpcszMd5,文件MD5是否正确
//返 回 值: 文件MD5正确返回TRUE，错误返回FALSE
//==================================================================
	BOOL IsCorrectOfMd5(LPCTSTR lpcszFileName, LPCTSTR lpcszMd5);
};

#endif