#ifndef ERROR_INFO_H
#define ERROR_INFO_H


#ifdef LS_COMMON_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API 
#endif

typedef enum
{
	CUSTOM_ERROR = 1,				//自定义错误
	SYSTEM_ERROR,					//系统错误
	WINSOCK_ERROR,					//WINSOCK错误
}LS_ERROR_TYPE,*PLS_ERROR_TYPE;

#define	ERROR_FLAG_ERROR_INFO			1			//错误信息
#define	ERROR_FLAG_DEBUG_INFO			2			//调试信息
#define	ERROR_FLAG_LOG_FILE_INFO		4			//日志信息
#define	ERROR_FLAG_ALL					0xffffffff	//所有方式

typedef enum
{
	LS_DEBUG_MODE_NONE,	//非调试模式
	LS_DEBUG_MODE_NORMAL,	//普通调试模式
}LS_DEBUG_MODE,*PLS_DEBUG_MODE;

typedef struct _LS_ERROR_INFO
{
	DWORD dwErrorType;
	DWORD dwErrorCode;
	CString szErrorInfo;
}LS_ERROR_INFO,*PLS_ERROR_INFO;

LS_COMMON_API CString GetLastErrorInfo();

LS_COMMON_API void ReleaseThreadErrorInfo();

LS_COMMON_API void StartupProcessErrorInfo();

LS_COMMON_API void ReleaseProcessErrorInfo();

LS_COMMON_API CString& GetThreadErrorInfoString();

LS_COMMON_API BOOL IsSpecialError(DWORD dwErrorType,DWORD dwErrorCode);

LS_COMMON_API DWORD SetErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

LS_COMMON_API DWORD SetErrorInfoV(LS_ERROR_INFO& LsErrorInfo , DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,va_list argList);

LS_COMMON_API DWORD SetErrorTitle(LPCTSTR lpszTitle,...);

//清空当前错误信息
LS_COMMON_API void ResetErrorInfo();

//追加错误信息
LS_COMMON_API void AppendErrorInfo(DWORD dwErrorType,DWORD dwErrorCode,LPCTSTR lpszErrorTitle,...);

//设置调试模式
LS_COMMON_API void SetDebugMode(LS_DEBUG_MODE dwDebugMode);

//获取默认日志文件路径
LS_COMMON_API CString GetDefaultLogFileName();

LS_COMMON_API BOOL WriteErrorInfoToLogFile(LPCTSTR lpszLogInfo = NULL ,...);

#define OutputStringToDebuger(lpszDebugTitle,...) OutputStringToDebugerEx(__FILE__ , __LINE__ , __FUNCTION__ , lpszDebugTitle , ## __VA_ARGS__)

LS_COMMON_API BOOL OutputStringToDebugerEx(LPCSTR lpszFileName , DWORD dwLine , LPCSTR lpszFunctionName , LPCTSTR lpszDebugInfo = NULL ,...);

#endif