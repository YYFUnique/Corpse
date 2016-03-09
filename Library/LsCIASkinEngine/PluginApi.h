#ifndef ___NSIS_PLUGIN__H___
#define ___NSIS_PLUGIN__H___

#ifdef __cplusplus
extern "C" {
#endif

#include "api.h"

#ifndef NSISCALL
#  define NSISCALL __stdcall
#endif

	typedef struct _stack_t {
		struct _stack_t *pNext;
		TCHAR szText[1]; // this should be the length of string_size
	} StackInfo;

	enum
	{
		INST_0,         // $0
		INST_1,         // $1
		INST_2,         // $2
		INST_3,         // $3
		INST_4,         // $4
		INST_5,         // $5
		INST_6,         // $6
		INST_7,         // $7
		INST_8,         // $8
		INST_9,         // $9
		INST_R0,        // $R0
		INST_R1,        // $R1
		INST_R2,        // $R2
		INST_R3,        // $R3
		INST_R4,        // $R4
		INST_R5,        // $R5
		INST_R6,        // $R6
		INST_R7,        // $R7
		INST_R8,        // $R8
		INST_R9,        // $R9
		INST_CMDLINE,   // $CMDLINE
		INST_INSTDIR,   // $INSTDIR
		INST_OUTDIR,    // $OUTDIR
		INST_EXEDIR,    // $EXEDIR
		INST_LANG,      // $LANGUAGE
		__INST_LAST
	};

	extern UINT				g_nStringSize;		//默认字符串缓冲区大小
	extern StackInfo**   g_pStacktop;		//栈顶指针
	extern LPTSTR		g_pVariables;		//用户自定义数据

	// 0 on success, 1 on empty stack
	/************************************************************************/
	/* 函数名称：PopString	                                                                           */
	/* 函数功能：弹出一个字符串																	   */
	/* 函数参数：strPopValue 返回的字符串													   */
	/* 返  回  值：成功返回TRUE，失败返回FALSE											  */
	/************************************************************************/
	BOOL NSISCALL PopString(CString& strPopValue); 

	/************************************************************************/
	/* 函数名称：PopStringn	                                                                           */
	/* 函数功能：弹出一个指定长度的字符串													   */
	/* 函数参数：strPopValue 返回的字符串													   */
	/*					 nMaxLen 字符串缓冲区长度，如果为0，则使用默认长度   	   */
	/* 返  回  值：成功返回TRUE，失败返回FALSE										 	  */
	/************************************************************************/
	BOOL NSISCALL PopStringn(CString& strPopValue, UINT nMaxLen = 0); // with length limit, pass 0 for g_stringsize

	/************************************************************************/
	/* 函数名称：PopInt	                                                                                   */
	/* 函数功能：弹出一个整数   																	   */
	/* 函数参数：szPopstr 字符串缓冲区														   */
	/* 返  回  值：返回栈顶整数																		  */
	/************************************************************************/
	BOOL NSISCALL PopInt(int& nPopValue);

// 	int NSISCALL popint_or(); // with support for or'ing (2|4|8)
// 	int NSISCALL myatoi(const char *s); // converts a string to an integer
// 	unsigned NSISCALL myatou(const char *s); // converts a string to an unsigned integer, decimal only
// 	int NSISCALL myatoi_or(const char *s); // with support for or'ing (2|4|8)

	/************************************************************************/
	/* 函数名称：PushString	                                                                           */
	/* 函数功能：将一个字符串压入栈																   */
	/* 函数参数：lpszString																			   */
	/* 返  回  值：执行成功返回TURE，否则返回FALSE									  */
	/************************************************************************/
	BOOL NSISCALL PushString(LPCTSTR lpszString);

	/************************************************************************/
	/* 函数名称：PushInt			                                                                       */
	/* 函数功能：将一个整数压入栈																   */
	/* 函数参数：nValue																				   */
	/* 返  回  值：返回栈顶整数																		  */
	/************************************************************************/
	BOOL NSISCALL PushInt(int nValue);

	/************************************************************************/
	/* 函数名称：GetUserVariable                                                                   */
	/* 函数功能：获取用户自定义数据																   */
	/* 函数参数：nVarNum																			  */
	/* 返  回  值：返回自定义数据指针															  */
	/************************************************************************/
	LPCTSTR NSISCALL GetUserVariable(int nVarNum);

	/************************************************************************/
	/* 函数名称：SetUserVariable                                                                   */
	/* 函数功能：社用户自定义数据																   */
	/* 函数参数：nVarNum																			  */
	/*				 ：lpszUserVariable 保存的内容												  */
	/* 返  回  值：无															  */
	/************************************************************************/
	void NSISCALL SetUserVariable(int nVarNum, LPCTSTR lpszUserVariable);

#ifdef __cplusplus
}
#endif

#endif//!___NSIS_PLUGIN__H___
