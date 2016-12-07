#pragma once


#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//==================================================================
//函 数 名: memxor
//功    能: 数据异或加密,key为-1
//输入参数: [in,out] dest, 数据首地址
//			[in]	 count, 数据大小
//返 回 值: 函数无返回值
//==================================================================
DLL_API void memxor(void* dest,size_t count);

//==================================================================
//函 数 名: XOROperate
//功    能: 将input中的内容，全部做异或(XOR)运算
//输入参数: 要进行异或的目标内容
//				 与目标内容进行异或的参数
//返 回 值：成功返回TRUE，失败返回FALSE
//==================================================================
DLL_API BOOL XOROperate(TCHAR *Input , DWORD dwOperateNum);