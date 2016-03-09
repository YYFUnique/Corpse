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

//����Ϊ����ģʽ��Ϣ������
//--------------------------------------------------------------------
#define DEBUG_MODE_NONE								0 //�ǵ���ģʽ
#define DEBUG_MODE_NORMAL							1 //��ͨ����ģʽ

#define DEBUG_INFO_OUTPUT_MODE_DEBUGGER				0x0001 //�����������
#define DEBUG_INFO_OUTPUT_MODE_FILE							0x0010	 //������ļ�
#define DEBUG_INFO_OUTPUT_MODE_REDIRECT				0x0100	 //�����Զ���ض���

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
// �������ƣ�LsSetUnhandledExceptionFilter
// �������ܣ�����ϵͳ����ʱ���õĴ�����
// ����������lpTopLevelExceptionFilter ����ָ��
// ��  �� ֵ���ú���Ŀǰ����TRUE�����򷵻�FALSE
/************************************************************************/
LS_COMMON_API BOOL LsSetUnhandledExceptionFilter( __in LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter );

//==================================================================
//�� �� ��: LsUnhandledExceptionFilter
//��    ��: ϵͳ�쳣������
//�������: [in]	_EXCEPTION_POINTERS
//						�����쳣ʱϵͳ����Ľṹ��ָ��
//�� �� ֵ: ���ص�ǰ�쳣���������Ĭ�Ϸ���EXCEPTION_EXECUTE_HANDLER
//==================================================================
LS_COMMON_API LONG WINAPI LsUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);

LS_COMMON_API BOOL GetCallstackInfo(const CONTEXT* pContext,CCallstackInfo& CallstackInfoList);