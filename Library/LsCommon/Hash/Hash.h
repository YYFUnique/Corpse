#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#define HASH_TYPE_MD5     1
#define HASH_TYPE_CRC32   2
#define HASH_TYPE_SHA1    3
#define HASH_TYPE_SHA256  4

#define HASH_MAX 128

class LS_COMMON_API CHash
{ 
protected:
	int m_nHashValueLen;
public:

//==================================================================
//函 数 名: HashType
//功    能: 设置Hash加密类型
//输入参数: [in] nHashType, Hash加密类型
//					HASH_TYPE_MD5, MD5
//					HASH_TYPE_CRC32, RC32
//					HASH_TYPE_SHA1, HA1
//返 回 值: 设置成功返回对应算法类指针,否则返回0
//==================================================================
	static CHash* HashType(int nHashType);

//==================================================================
//函 数 名: Init
//功    能: Hash算法初始操作(纯虚函数)
//输入参数: 无输入参数
//返 回 值: 无返回值
//==================================================================
	virtual  void Init()=0;

//==================================================================
//函 数 名: Update
//功    能: 输入加密数据(纯虚函数)
//输入参数: [in] pBuffer, 加密数据缓存地址
//			[in] dwSize, 加密数据长度
//返 回 值: 无返回值
//==================================================================
	virtual void Update(BYTE* pBuffer, DWORD dwSize)=0;

//==================================================================
//函 数 名: Final
//功    能: 获得加密后的HASH值(纯虚函数)
//输入参数: [out] pHashvalue, Hash值
//返 回 值: 返回HASH值的位数
//==================================================================
	virtual DWORD Final(BYTE* pHashvalue)=0;

//==================================================================
//函 数 名: GetFileHash
//功    能: 获得文件的HASH值
//输入参数: [in]  lpcszName, 全路径文件名
//			[out] pHashvalue, HASH值
//返 回 值: 获取文件HASH值成功返回TRUE,否则返回FALSE
//==================================================================
	virtual BOOL GetFileHash(LPCTSTR lpcszName, BYTE* pHashvalue);

//==================================================================
//函 数 名: GetHash
//功    能: 对特定信息加密
//输入参数: [in]  nHashType,HASH算法类型
//					HASH_TYPE_MD5, MD5
//					HASH_TYPE_CRC32, RC32
//					HASH_TYPE_SHA1, HA1
//			[in]  pBuffer,加密数据地址
//			[in]  dwSize, 加密数据大小
//			[out]  szHashText, 字符串格式HASH值
//返 回 值: 无返回值
//==================================================================
	static void GetHash(int nHashType, BYTE* pBuffer, DWORD dwSize, LPTSTR szHashText);

//==================================================================
//函 数 名: GetHash
//功    能: 对特定信息加密
//输入参数: [in]  nHashType,HASH算法类型
//					HASH_TYPE_MD5, MD5
//					HASH_TYPE_CRC32, RC32
//					HASH_TYPE_SHA1, HA1
//			[in]  pBuffer,加密数据地址
//			[in]  dwSize, 加密数据大小
//			[out] pHashvalue, HASH值
//返 回 值: 返回HASH值长度
//==================================================================
	static DWORD GetHash(int nHashType, BYTE* pBuffer, DWORD dwSize, BYTE* pHashvalue); 
	
//==================================================================
//函 数 名: GetBigEndianResult
//功    能: 将HASH值字节序从主机字节序转换为网络字节序
//输入参数: [in,out] 传入主机字节序HASH值,传出网络字节序HASH值
//返 回 值: 无输入参数
//==================================================================
	virtual void GetBigEndianResult(BYTE* pHashValue);
	
//==================================================================
//函 数 名: GetHashHexText
//功    能: 将HASH值转换为字符串形式
//输入参数: [in]  pHashBuffer, 传入HASH值缓存地址
//				[out] szHashText, 传出字符串HASH值
//			    nLen 缓冲区长度
//返 回 值: 无返回值
//==================================================================
	virtual void GetHashHexText(const BYTE* pHashBuffer,LPTSTR szHashText);

//==================================================================
//函 数 名: GetHashHexText
//功    能: 将HASH值转换为CStringg格式
//输入参数: [in]  pHashBuffer, 传入HASH值缓存地址
//			[out] szHashText, 传出CString格式HASH值
//返 回 值: 成功返回TRUE
//==================================================================
	virtual void GetHashHexText(const BYTE* pHashBuffer,CString& szHashText);

//==================================================================
//函 数 名: Release
//功    能: 调用类析构函数释放内存
//输入参数: 无输入参数
//返 回 值: 无返回值
//==================================================================
	virtual void Release();

};



//void GetHashHexText(const BYTE* pHashBuffer,CString& szHashText);
//
//virtual DWORD GetBigEndianResult(void* pHashBuffer);