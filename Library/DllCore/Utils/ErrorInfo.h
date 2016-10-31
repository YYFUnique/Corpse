#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <atlstr.h>

#ifndef NT_SUCCESS
#define NTSTATUS LONG
#endif
//

typedef ULONG (WINAPI *LPFN_RtlDosErrorFromNtStatus)(IN NTSTATUS Status);

//错误信息线程本地存储
typedef enum
{
	CUSTOM_ERROR = 1,			//自定义错误
	SYSTEM_ERROR,					//系统错误
	WINSOCK_ERROR,				//WINSOCK错误
	KERNEL_ERROR,					//NTSTATUS类型的错误
	COM_ERROR,						//COM返回的LRESULT错误，可以使用HRESULT_CODE取得错误号
}LS_ERROR_TYPE,*PLS_ERROR_TYPE;

typedef struct _ERROR_INFO
{
	DWORD dwErrorType;
	DWORD dwErrorCode;
	CString   strErrorInfo;
}ERROR_INFO,*PERROR_INFO;

//==================================================================
//函 数 名: GetLastErrorInfo
//功    能: 获取最后错误信息
//输入参数: 无
//返 回 值: 返回错误信息的描述字符串
//==================================================================
DLL_API CString GetLastErrorInfo();

//==================================================================
//函 数 名: ReleaseThreadErrorInfo
//功    能: 释放线程错误信息内存
//输入参数: 无
//返 回 值: 无返回值
//==================================================================
DLL_API void ReleaseThreadErrorInfo();

//==================================================================
//函 数 名: ReleaseProcessErrorInfo
//功    能: 释放进程错误信息相关资源
//输入参数: 无
//返 回 值: 无返回值
//==================================================================
DLL_API void ReleaseProcessErrorInfo();

//==================================================================
//函 数 名: GetThreadErrorInfoString
//功    能: 获取线程错误消息字符串
//输入参数: 无
//返 回 值: 返回线程错误消息字符串的引用
//==================================================================
DLL_API CString& GetThreadErrorInfoString();

//==================================================================
//函 数 名: IsSpecialError
//功    能: 判断当前错误是否为指定的错误
//输入参数: [in]  dwErrorType, 错误类型,分为:
//			SYSTEM_ERROR, 系统错误
//			CUSTOM_ERROR, 自定义错误
//			[in]  dwErrorCode, 错误代码
//返 回 值: 是指定的错误返回TRUE,否则FALSE 
//==================================================================
DLL_API BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode);

//==================================================================
//函 数 名: SetErrorInfo
//功    能: 设置错误信息
//输入参数: [in]  dwErrorType, 错误类型,分为:
//			SYSTEM_ERROR, 系统错误
//			CUSTOM_ERROR, 自定义错误
//			WINSOCK_ERROR,网络中的一些错误
//			KERNEL_ERROR,	NT系统中的错误，使用该错误的时候，必须传入NT系统的错误号
//			[in]  dwErrorCode, 错误代码,该参数如果为0,则自动获取一个错误代码
//			[in]  lpszErrorTitle, 错误消息title控制字符串
//			[in]  ..., 参数列表
//返 回 值: 返回错误代码
//==================================================================
DLL_API DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

DLL_API DWORD RtlNtStatusToDosError(NTSTATUS NtStatus);

//==================================================================
//函 数 名: SetErrorInfo
//功    能: 设置错误信息
//==================================================================
DWORD SetErrorInfoV(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList);

//==================================================================
//函 数 名: SetErrorTitle
//功    能: 设置错误信息的前面的提示信息
//输入参数: [in]  lpszTitle, 错误消息前面的提示信息
//			(例如:"打开文件失败:另一个程序正在使用此文件，进程无法访问",
//			这里设置的就是前面的"打开文件失败"冒号之前的字符串)
//返 回 值: 返回错误代码
//==================================================================
DLL_API DWORD SetErrorTitle(LPCTSTR lpszTitle,...);

//==================================================================
//函 数 名: ResetErrorInfo
//功    能: 清空当前的错误信息
//输入参数: 无
//返 回 值: 无
//==================================================================
DLL_API void ResetErrorInfo();

//==================================================================
//函 数 名: AppendErrorInfo
//功    能: 追加错误信息,每次调用产生的错误结果以回车换行符为分割
//输入参数: 参见SetErrorInfo函数
//返 回 值: 无
//==================================================================
DLL_API void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

CString GetLogModuleFileName();