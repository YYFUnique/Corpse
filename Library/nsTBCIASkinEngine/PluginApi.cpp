#include "stdafx.h"
#include "PluginApi.h"

UINT			g_nStringSize;		//默认字符串缓冲区大小
StackInfo**  g_pStacktop;		//栈顶指针
LPTSTR		g_pVariables;		//用户自定义数据

BOOL NSISCALL PopString(CString& strPopValue)
{
	if (g_pStacktop == NULL || *g_pStacktop == NULL)
		return FALSE;

	StackInfo* pStackInfo  = *g_pStacktop;
	strPopValue = pStackInfo->szText;
	*g_pStacktop = pStackInfo->pNext;
	GlobalFree((HGLOBAL)pStackInfo);

	return TRUE;
}

BOOL NSISCALL PopStringn(CString& strPopValue, UINT nMaxLen/* = 0*/)
{
	if (g_pStacktop == NULL || *g_pStacktop == NULL)
		return FALSE;

	StackInfo* pStackInfo  = *g_pStacktop;
	UINT nLen = nMaxLen == 0 ? g_nStringSize : nMaxLen;
	CString strPopTmp(pStackInfo->szText,nLen);
	strPopValue = strPopTmp;
	*g_pStacktop = pStackInfo->pNext;
	GlobalFree((HGLOBAL)pStackInfo);

	return TRUE;
}

BOOL NSISCALL PushString(LPCTSTR lpszString)
{
	if (g_pStacktop == NULL)
		return FALSE;

	StackInfo *pStackInfo =(StackInfo*)GlobalAlloc(GPTR,sizeof(StackInfo) + g_nStringSize);
	_tcscpy_s(pStackInfo->szText,g_nStringSize,lpszString);
	pStackInfo->pNext=*g_pStacktop;
	*g_pStacktop=pStackInfo;

	return TRUE;
}

BOOL NSISCALL PopInt(int& nPopValue)
{
	CString strPopInt;
	if (PopString(strPopInt) == FALSE)
		return FALSE;

	int nRet = _stscanf_s(strPopInt,_T("%d"),&nPopValue);

	return nRet == EOF ? TRUE : FALSE;
}

BOOL NSISCALL PushInt(int nValue)
{
	CString strIntValue;
	strIntValue.Format(_T("%d"),nValue);

	return PushString(strIntValue);
}

LPCTSTR NSISCALL GetUserVariable(int nVarNum)
{
	if (nVarNum < 0 || nVarNum >= __INST_LAST)
		return NULL;

	return g_pVariables+nVarNum*g_nStringSize;
}

void NSISCALL SetUserVariable(int nVarNum, LPCTSTR lpszUserVariable)
{
	if (lpszUserVariable != NULL && nVarNum >= 0 && nVarNum < __INST_LAST) 
		_tcscpy(g_pVariables + nVarNum*g_nStringSize, lpszUserVariable);
}