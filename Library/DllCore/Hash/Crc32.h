#include "Hash.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define MAX_BUFFER_SIZE	4096

class DLL_API CCRC32: public CHash
{
public:
	CCRC32();
private:
	DWORD dwCrc32;
public:

//==================================================================
//函 数 名: Init
//功    能: 初始化CRC32算法
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
//函 数 名: Final
//功    能: 获取经CRC32算法计算后HASH值
//输入参数: [out], pHashvalue, HASH值
//返 回 值: 返回HASH值的位数
//==================================================================
	DWORD Final(BYTE* pHashvalue);

//==================================================================
//函 数 名: GetBigEndianResult
//功    能: 将本地字节序转换为网络字节序
//输入参数: [in,out] pHashValue, 输入本地字节序HASH值,输出网络字节序HASH值
//返 回 值: 无返回值
//==================================================================
	void GetBigEndianResult(BYTE* pHashValue); 

//==================================================================
//函 数 名: GetHashHexText
//功    能: 将HASH值转换为16进制字符串格式
//输入参数: [in]  pHashBuffer, (未使用)
//			[out] szHashText, 输出16进制字符串格式HASH值
//返 回 值: 无返回值
//==================================================================
	virtual void GetHashHexText(const BYTE* pHashBuffer,LPTSTR szHashText);   

//==================================================================
//函 数 名: GetHashHexText
//功    能: 将HASH值转换为16进制CString格式
//输入参数: [in]  pHashBuffer, 输入待转换HASH值
//			[out] szHashText, 16进制CString格式HASH值
//返 回 值: 无返回值
//==================================================================
	virtual void GetHashHexText(const BYTE* pHashBuffer,CString& szHashText);
	
//==================================================================
//函 数 名: HashBuffer
//功    能: 对指定缓存进行HASH运算
//输入参数: [in] pHashBuffer, 进行HASH运算的缓存地址
//			[in] dwBufferSize, 进行HASH运算的缓存大小
//返 回 值: 返回运算得到的HASH值
//==================================================================
	static DWORD HashBuffer(const void* pHashBuffer,DWORD dwBufferSize);

private:

//==================================================================
//函 数 名: CalcCrc32
//功    能: CRC32加密算法
//输入参数: [in] byte, 加密数据缓存地址
//			[in] length, 加密数据缓存大小
//			[out] dwCrc32, 加密后所得的密文
//返 回 值: 无返回值
//==================================================================
	void CalcCrc32(const BYTE* byte, DWORD length, DWORD &dwCrc32);

protected:
	static const DWORD Crc32Table[256];
};