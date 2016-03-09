#pragma once

// 原来LanGuardError中的错误定义
#define ERROR_LG_SUCCESS				0				// 成功
#define ERROR_LG_LOAD_PROFILE			1				// 从配置文件中加载规则失败
#define ERROR_LG_SAVE_PROFILE			2				// 保存规则到配置文件失败
#define ERROR_LG_CHECK_PROFILE			3				// 规则配置文件已损坏
#define ERROR_LG_INVALID_PARAMETER		4				// 无效的规则参数
#define ERROR_LG_SCM					5				// 打开服务管理器失败
#define ERROR_LG_SERVICE				6				// 创建服务失败
#define ERROR_LG_MEMORY					7				// 保留内存失败
#define ERROR_LG_INVALID_PATH_OR_FILE	8				// 非法的文件路径
#define ERROR_LG_INVALID_HANDLE			9				// 非法的驱动句柄
#define ERROR_LG_STARTING				10				// 启动驱动程序失败
#define ERROR_LG_STOPPING				11				// 停止驱动程序失败
#define ERROR_LG_REMOVING				12				// 删除驱动程序服务失败
#define ERROR_LG_IO						13				// 对驱动操作失败
#define ERROR_LG_NO_INITIALIZED			14				// 没有初始化
#define ERROR_LG_ALREADY_INITIALIZED	15				// 已经初始化
#define ERROR_LG_NULL_POINTER			16				// 指针是空指针
#define ERROR_LG_GET_LOCAL_ADDRESS		17				// 获取本机地址失败
#define ERROR_LG_GET_GATE_WAY			18				// 获取默认网关失败
#define ERROR_LG_REMOVE_NET_RULE		19				// 删除网络规则失败
#define ERROR_LG_SET_NET_RULE			20				// 设置网络规则失败

#ifndef NT_SUCCESS
#define NTSTATUS LONG
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
CString GetLastErrorInfo();

//==================================================================
//函 数 名: ReleaseThreadErrorInfo
//功    能: 释放线程错误信息内存
//输入参数: 无
//返 回 值: 无返回值
//==================================================================
void ReleaseThreadErrorInfo();

//==================================================================
//函 数 名: StartupProcessErrorInfo
//功    能: 设置错误信息保存路径
//输入参数: [in]  lpszLogFileName, 错误信息日志文件路径
//返 回 值: 无返回值
//==================================================================
void StartupProcessErrorInfo(LPCTSTR lpszLogFileName=NULL);

//==================================================================
//函 数 名: ReleaseProcessErrorInfo
//功    能: 释放进程错误信息相关资源
//输入参数: 无
//返 回 值: 无返回值
//==================================================================
void ReleaseProcessErrorInfo();

//==================================================================
//函 数 名: GetThreadErrorInfoString
//功    能: 获取线程错误消息字符串
//输入参数: 无
//返 回 值: 返回线程错误消息字符串的引用
//==================================================================
CString& GetThreadErrorInfoString();

//==================================================================
//函 数 名: IsSpecialError
//功    能: 判断当前错误是否为指定的错误
//输入参数: [in]  dwErrorType, 错误类型,分为:
//			SYSTEM_ERROR, 系统错误
//			CUSTOM_ERROR, 自定义错误
//			[in]  dwErrorCode, 错误代码
//返 回 值: 是指定的错误返回TRUE,否则FALSE 
//==================================================================
BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode);

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
DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

DWORD RtlNtStatusToDosError(NTSTATUS NtStatus);

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
DWORD SetErrorInfoV(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList);

//==================================================================
//函 数 名: SetErrorTitle
//功    能: 设置错误信息的前面的提示信息
//输入参数: [in]  lpszTitle, 错误消息前面的提示信息
//			(例如:"打开文件失败:另一个程序正在使用此文件，进程无法访问",
//			这里设置的就是前面的"打开文件失败"冒号之前的字符串)
//返 回 值: 返回错误代码
//==================================================================
DWORD SetErrorTitle(LPCTSTR lpszTitle,...);

//==================================================================
//函 数 名: ResetErrorInfo
//功    能: 清空当前的错误信息
//输入参数: 无
//返 回 值: 无
//==================================================================
void ResetErrorInfo();

//==================================================================
//函 数 名: AppendErrorInfo
//功    能: 追加错误信息,每次调用产生的错误结果以回车换行符为分割
//输入参数: 参见SetErrorInfo函数
//返 回 值: 无
//==================================================================
void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

//==================================================================
//函 数 名: WriteErrorInfoToLogFile
//功    能: 把错误信息写入指定的日志文件
//输入参数: [in]  lpszLogInfo, 写入日志的信息
//返 回 值: 写入日志文件成功返回TRUE, 否则返回FALSE
//==================================================================
BOOL WriteErrorInfoToLogFile(LPCTSTR lpszLogInfo=NULL,...);

//==================================================================
//函 数 名: GetLogFileName
//功    能: 获取日志文件路径,(例如C:\Documents and Settings\All Users.WINDOWS\
//			Application Data\Lonsin\龙信终端安全管理系统\Log\AssetMgr(2009-09-25).log)
//输入参数: 无
//返 回 值: 返回获取到的日志文件的路径
//==================================================================
CString GetLogFileName();

//==================================================================
//函 数 名: GetAppDataFileName
//功    能: 获取日志文件路径,(例如C:\Documents and Settings\All Users.WINDOWS\
//			Application Data\Lonsin\龙信终端安全管理系统\"lpszDirName"\文件名."lpszExtendName"
//输入参数: 
//				lpszDirName			要获取的lpszDirName文件夹路径
//				lpszExtendName		获取lpszExtendName类型的文件
//				lpszVersion				用于添加系统主程序版本号
//返 回 值: 返回获取到的日志文件的路径
//==================================================================
CString GetLogFilePath(LPCTSTR lpszDirName,LPCTSTR lpszExtendName,LPCTSTR lpszVersion = NULL);

//==================================================================
//函 数 名: SetLogFileCodeType
//功    能: 设置日志文件编码方式
//输入参数: [in]  dwCodeType
//返 回 值: 设置编码方式成功返回TRUE, 如果日志文件已经打开则返回FALSE
//==================================================================
BOOL SetLogFileCodeType(DWORD dwCodeType);

extern DWORD g_dwLastError;

CString GetLogModuleFileName();