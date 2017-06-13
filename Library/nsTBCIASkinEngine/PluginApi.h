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

	extern UINT				g_nStringSize;		//Ĭ���ַ�����������С
	extern StackInfo**   g_pStacktop;		//ջ��ָ��
	extern LPTSTR		g_pVariables;		//�û��Զ�������

	// 0 on success, 1 on empty stack
	/************************************************************************/
	/* �������ƣ�PopString	                                                                           */
	/* �������ܣ�����һ���ַ���																	   */
	/* ����������strPopValue ���ص��ַ���													   */
	/* ��  ��  ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE											  */
	/************************************************************************/
	BOOL NSISCALL PopString(CString& strPopValue); 

	/************************************************************************/
	/* �������ƣ�PopStringn	                                                                           */
	/* �������ܣ�����һ��ָ�����ȵ��ַ���													   */
	/* ����������strPopValue ���ص��ַ���													   */
	/*					 nMaxLen �ַ������������ȣ����Ϊ0����ʹ��Ĭ�ϳ���   	   */
	/* ��  ��  ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE										 	  */
	/************************************************************************/
	BOOL NSISCALL PopStringn(CString& strPopValue, UINT nMaxLen = 0); // with length limit, pass 0 for g_stringsize

	/************************************************************************/
	/* �������ƣ�PopInt	                                                                                   */
	/* �������ܣ�����һ������   																	   */
	/* ����������szPopstr �ַ���������														   */
	/* ��  ��  ֵ������ջ������																		  */
	/************************************************************************/
	BOOL NSISCALL PopInt(int& nPopValue);

// 	int NSISCALL popint_or(); // with support for or'ing (2|4|8)
// 	int NSISCALL myatoi(const char *s); // converts a string to an integer
// 	unsigned NSISCALL myatou(const char *s); // converts a string to an unsigned integer, decimal only
// 	int NSISCALL myatoi_or(const char *s); // with support for or'ing (2|4|8)

	/************************************************************************/
	/* �������ƣ�PushString	                                                                           */
	/* �������ܣ���һ���ַ���ѹ��ջ																   */
	/* ����������lpszString																			   */
	/* ��  ��  ֵ��ִ�гɹ�����TURE�����򷵻�FALSE									  */
	/************************************************************************/
	BOOL NSISCALL PushString(LPCTSTR lpszString);

	/************************************************************************/
	/* �������ƣ�PushInt			                                                                       */
	/* �������ܣ���һ������ѹ��ջ																   */
	/* ����������nValue																				   */
	/* ��  ��  ֵ������ջ������																		  */
	/************************************************************************/
	BOOL NSISCALL PushInt(int nValue);

	/************************************************************************/
	/* �������ƣ�GetUserVariable                                                                   */
	/* �������ܣ���ȡ�û��Զ�������																   */
	/* ����������nVarNum																			  */
	/* ��  ��  ֵ�������Զ�������ָ��															  */
	/************************************************************************/
	LPCTSTR NSISCALL GetUserVariable(int nVarNum);

	/************************************************************************/
	/* �������ƣ�SetUserVariable                                                                   */
	/* �������ܣ����û��Զ�������																   */
	/* ����������nVarNum																			  */
	/*				 ��lpszUserVariable ���������												  */
	/* ��  ��  ֵ����															  */
	/************************************************************************/
	void NSISCALL SetUserVariable(int nVarNum, LPCTSTR lpszUserVariable);

#ifdef __cplusplus
}
#endif

#endif//!___NSIS_PLUGIN__H___
