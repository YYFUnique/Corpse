#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMONEX_API
#elif defined LS_COMMONEX_EXPORTS
#define LS_COMMONEX_API __declspec(dllexport)
#else
#define LS_COMMONEX_API 
#endif

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
	CString szErrorInfo;
}ERROR_INFO,*PERROR_INFO;

//==================================================================
//函 数 名: GetLastErrorInfo
//功    能: 获取最后错误信息
//输入参数: 无
//返 回 值: 返回错误信息的描述字符串
//==================================================================
LS_COMMONEX_API CString GetLastErrorInfo();

//==================================================================
//函 数 名: ReleaseThreadErrorInfo
//功    能: 释放线程错误信息内存
//输入参数: 无
//返 回 值: 无返回值
//==================================================================
LS_COMMONEX_API void ReleaseThreadErrorInfo();

//==================================================================
//函 数 名: StartupProcessErrorInfo
//功    能: 设置错误信息保存路径
//输入参数: [in]  lpszLogFileName, 错误信息日志文件路径
//返 回 值: 无返回值
//==================================================================
LS_COMMONEX_API void StartupProcessErrorInfo(LPCTSTR lpszLogFileName=NULL);

//==================================================================
//函 数 名: ReleaseProcessErrorInfo
//功    能: 释放进程错误信息相关资源
//输入参数: 无
//返 回 值: 无返回值
//==================================================================
LS_COMMONEX_API void ReleaseProcessErrorInfo();

//==================================================================
//函 数 名: GetThreadErrorInfoString
//功    能: 获取线程错误消息字符串
//输入参数: 无
//返 回 值: 返回线程错误消息字符串的引用
//==================================================================
LS_COMMONEX_API CString& GetThreadErrorInfoString();

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
LS_COMMONEX_API DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

//==================================================================
//函 数 名: SetErrorInfo
//功    能: 设置错误信息
//输入参数: [in]  dwErrorType, 错误类型,分为:
//			SYSTEM_ERROR, 系统错误
//			CUSTOM_ERROR, 自定义错误
//			[in]  dwErrorCode, 错误代码
//			[in]  lpszErrorTitle, 错误消息title控制字符串
//			[in]  argList, 参数列表
//返 回 值: 返回错误代码
//==================================================================
LS_COMMONEX_API DWORD SetErrorInfoV(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList);

//==================================================================
//函 数 名: SetErrorTitle
//功    能: 设置错误信息的前面的提示信息
//输入参数: [in]  lpszTitle, 错误消息前面的提示信息
//			(例如:"打开文件失败:另一个程序正在使用此文件，进程无法访问",
//			这里设置的就是前面的"打开文件失败"冒号之前的字符串)
//返 回 值: 返回错误代码
//==================================================================
LS_COMMONEX_API DWORD SetErrorTitle(LPCTSTR lpszTitle,...);
