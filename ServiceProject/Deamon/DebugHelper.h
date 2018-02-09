#pragma once

#include "DllCore/DllCore.h"

//���������Ϣ�������
typedef enum tagDBG_TRACE_FLAG{
	DBG_TRACE_FLAG_NONE				= 0,		//����ʾ�κ���Ϣ
	DBG_TRACE_FLAG_INFO					= 1,		//��ʾһ����Ϣ
	DBG_TRACE_FLAG_NOTICE			= 2,		//��ʾ��ʾ��Ϣ
	DBG_TRACE_FLAG_DEBUG				= 4,		//��ʾ������Ϣ
	DBG_TRACE_FLAG_WARNNING		= 8,		//��ʾ������Ϣ
	DBG_TRACE_FLAG_ERROR				= 16,	//��ʾ������Ϣ
	DBG_TRACE_FLAG_ALL					= -1,		//��ʾ������Ϣ
}DBG_TRACE_FLAG;

EXTERN_C DBG_TRACE_FLAG gTraceFlags;

void DbgPrint(LPCTSTR lpszFormatString, ...);

DBG_TRACE_FLAG SetDbgTraceFlag(DBG_TRACE_FLAG DbgTraceFlag);

#ifdef _DEBUG
	#define LOG_PRINT( _dbgLevel, _string, ... )		 \
			(FlagOn(gTraceFlags,(_dbgLevel))	?			 \
					DbgPrint(_string, ##__VA_ARGS__)	 :	 \
					((int)0))
#else
	#define LOG_PRINT( _dbgLevel, _string, ...)	((int)0)
#endif