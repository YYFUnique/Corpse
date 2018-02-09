#include "StdAfx.h"
#include "DebugHelper.h"

 DBG_TRACE_FLAG gTraceFlags = DBG_TRACE_FLAG_ALL;

void DbgPrint(LPCTSTR lpszFormatString, ...)
{
	BOOL    bResult = TRUE;

	va_list vaList;
	va_start(vaList, lpszFormatString);

	CString strTipInfo;
	strTipInfo.Format(lpszFormatString, vaList);
	OutputDebugString(strTipInfo);

	va_end(vaList);
}

DBG_TRACE_FLAG SetDbgTraceFlag(DBG_TRACE_FLAG DbgTraceFlag)
{
	DWORD dwTraceFlag = gTraceFlags;
	DBG_TRACE_FLAG gTraceFlags = DbgTraceFlag;

	return (DBG_TRACE_FLAG)dwTraceFlag;
}