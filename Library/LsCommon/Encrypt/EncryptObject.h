#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#define LS_ENCRYPT_ALG_AES	1

/////////////////////////////////////////////////////////////////////////////
// CEncryptObject window

class LS_COMMON_API CEncryptObject
{
// Construction
public:
	CEncryptObject();
	enum { ECB=0, CBC=1, CFB=2 };
// Attributes
public:

//==================================================================
//函 数 名: GetKeyLength
//功    能: 获取密钥的长度(纯虚函数)
//输入参数: 无输入参数
//返 回 值: 返回密钥长度
//==================================================================
	virtual int GetKeyLength()=0;//Get Key Length

//==================================================================
//函 数 名: GetBlockSize
//功    能: 获取加密块大小(纯虚函数)
//输入参数: 无输入参数
//返 回 值: 返回加密块大小
//==================================================================
	virtual int GetBlockSize()=0;//Block Size

// Operations
public:

//==================================================================
//函 数 名: Create
//功    能: 创建加密算法
//输入参数: [in] 加密算法ID
//					LS_ENCRYPT_ALG_AES Rijndael算法
//返 回 值: 创建成功返回指定加密算法对象,失败返回NULL
//==================================================================
	static CEncryptObject* Create(DWORD dwAlgId);

//==================================================================
//函 数 名: SetKey
//功    能: 扩展用户密钥为会话密钥(纯虚函数)			
//输入参数: [in] lpKey, 密钥
//			[in] lpChain, 数据块链
//			[in] nKeyLength, 密钥长度
//			[in] nBlockSize,数据块大小
//返 回 值: 无返回值
//==================================================================
	virtual void SetKey(const void* lpKey, const void* lpChain, int nKeyLength, int nBlockSize)=0;

//==================================================================
//函 数 名: ResetChain
//功    能: 重置数据块链(纯虚函数)
//输入参数: 无输入参数
//返 回 值: 无返回值
//==================================================================
	virtual void ResetChain()=0;

//==================================================================
//函 数 名: Encrypt
//功    能: 加密(纯虚函数)
//输入参数: [in]  in, 需加密内容缓存地址
//			[out] result, 加密后密文缓存地址
//			[in]  dwBufferLen, 加密内容或加密后密文缓存大小
//			[in]  iMode, 加密方式
//					ECB 电子代码书(Electronic Code Book)
//					CBC 链块编码(Chain Block Coding)
//					CFB	密码反馈(Cipher Feedback)
//返 回 值: 函数调用成功返回TRUE,否则返回FALSE
//==================================================================
	virtual BOOL Encrypt(const void* in, void* result, DWORD dwBufferLen, int iMode)=0;

//==================================================================
//函 数 名: Decrypt
//功    能: 解密(纯虚函数)
//输入参数: [in]  in, 需解密内容缓存地址
//			[out] result, 解密后数据缓存地址
//			[in]  dwBufferLen, 解密内容或解密后数据缓存大小
//			[in]  iMode, 解密方式
//					ECB 电子代码书(Electronic Code Book)
//					CBC 链块编码(Chain Block Coding)
//					CFB	密码反馈(Cipher Feedback)
//返 回 值: 函数调用成功返回TRUE,否则返回FALSE
//==================================================================
	virtual BOOL Decrypt(const void* in, void* result, DWORD dwBufferLen, int iMode)=0;

//==================================================================
//函 数 名: Release
//功    能: 释放内存
//输入参数: 无输入参数
//返 回 值: 无返回值
//==================================================================
	virtual void Release();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncryptObject)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEncryptObject();

	// Generated message map functions
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
