#pragma once

#include <afxcoll.h>
#include "afxtempl.h"
#include "Winsock2.h"

#include <Wininet.h>
#include <Dbghelp.h>

#pragma comment(lib , "Wininet.lib")
#pragma comment( lib, "DbgHelp.lib" )

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif


#define ASSERT_STRING_NOT_NONE(str)			ASSERT((str) != NULL && _tcslen(str) !=0 )

//以下为调试模式信息处理函数
//--------------------------------------------------------------------
#define DEBUG_MODE_NONE								0 //非调试模式
#define DEBUG_MODE_NORMAL							1 //普通调试模式

#define DEBUG_INFO_OUTPUT_MODE_DEBUGGER				0x0001 //输出到调试器
#define DEBUG_INFO_OUTPUT_MODE_FILE							0x0010	 //输出到文件
#define DEBUG_INFO_OUTPUT_MODE_REDIRECT				0x0100	 //输出到远程重定向

#ifdef MODULE_NAME
	#define OutputWhenDebugMode(lpszFormat,...) OutputEx(MODULE_NAME,__FILE__,__LINE__,lpszFormat,__VA_ARGS__)
#else
	#define OutputWhenDebugMode(lpszFormat,...) Output(__FILE__,__LINE__,lpszFormat,__VA_ARGS__)
#endif

#define OutputDebugStringWhenDebugMode OutputWhenDebugMode

typedef struct _tagCALLSTACKINFO
{
	CString strModuleName;
	CString strMethodName;
	CString strFileName;
	DWORD dwLine;
}CALLSTACKINFO,*PCALLSTACKINFO;

typedef CList<CALLSTACKINFO,const CALLSTACKINFO&> CCallstackInfo;

LS_COMMON_API void Output(LPCSTR lpszFile,int nLine,LPCTSTR lpszFormat,...);

LS_COMMON_API void OutputEx(LPCTSTR lpszModuleName,LPCSTR lpszFile,int nLine,LPCTSTR lpszFormat,...);

LS_COMMON_API void OutputV(LPCTSTR lpszModuleName,LPCTSTR lpszFileName,int nLine,LPCTSTR lpszMsg,va_list argList);

/************************************************************************/
// 函数名称：LsSetUnhandledExceptionFilter
// 函数功能：设置系统崩溃时调用的处理函数
// 函数参数：lpTopLevelExceptionFilter 函数指针
// 返  回 值：该函数目前返回TRUE，否则返回FALSE
/************************************************************************/
LS_COMMON_API BOOL LsSetUnhandledExceptionFilter( __in LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter );

//==================================================================
//函 数 名: LsUnhandledExceptionFilter
//功    能: 系统异常处理函数
//输入参数: [in]	_EXCEPTION_POINTERS
//						发生异常时系统传入的结构体指针
//返 回 值: 返回当前异常处理情况，默认返回EXCEPTION_EXECUTE_HANDLER
//==================================================================
LS_COMMON_API LONG WINAPI LsUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);

LS_COMMON_API BOOL GetCallstackInfo(const CONTEXT* pContext,CCallstackInfo& CallstackInfoList);