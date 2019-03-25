#pragma once

#include "DllCore/DllCore.h"

//定义调试信息输出类型
typedef enum tagDBG_TRACE_FLAG{
	DBG_TRACE_FLAG_NONE				= 0,		//不显示任何信息
	DBG_TRACE_FLAG_INFO					= 1,		//显示一般信息
	DBG_TRACE_FLAG_NOTICE			= 2,		//显示提示信息
	DBG_TRACE_FLAG_DEBUG				= 4,		//显示调试信息
	DBG_TRACE_FLAG_WARNNING		= 8,		//显示警告信息
	DBG_TRACE_FLAG_ERROR				= 16,	//显示错误信息
	DBG_TRACE_FLAG_ALL					= -1,		//显示所有信息
}DBG_TRACE_FLAG;

EXTERN_C DBG_TRACE_FLAG gTraceFlags;

void DbgPrint(LPCTSTR lpszFormatString, ...);

DBG_TRACE_FLAG SetDbgTraceFlag(DBG_TRACE_FLAG DbgTraceFlag);

#ifdef _DEBUG
	#define LOG_DBGPRINT( _dbgLevel, _string, ... )		 \
			(FlagOn(gTraceFlags,(_dbgLevel))	?			 \
					DbgPrint(_string, ##__VA_ARGS__)	 :	 \
					((int)0))
#else
	#define LOG_DBGPRINT( _dbgLevel, _string, ...)	((int)0)
#endif