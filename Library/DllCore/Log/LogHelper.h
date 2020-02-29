#pragma once

/*  结合c#与cout的log
 *  示例：
 *  QLOG_ERR(L"I am {0},this is {1} year,you can also call me {2}") <<L"天外飞仙" <<2015 <<"Tom Stiven";
 */

#ifdef LS_STATIC_LIB_CALL
#define DLL_API
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#pragma warning (disable : 4251)

#include <atlstr.h>
#include "../Sync/SyncLock.h"

typedef enum tagLOG_LEVEL
{
	LOG_LEVEL_FATAL  = 0,
	LOG_LEVEL_ERR		=	1,
	LOG_LEVEL_WAR	= 2,
	LOG_LEVEL_APP		= 3,
	LOG_LEVEL_PRO		= 4,
	LOG_LEVEL_INFO	= 5,
	LOG_LEVEL_DBG     = 6,
	LOG_LEVEL_ALL		= 7,
}LOG_LEVEL;

typedef enum LOG_TYPE
{
	LOG_TYPE_DISABLE	=	0,
	LOG_TYPE_TOFILE			=	1,
	LOG_TYPE_DBGVIEW	=	2,
}LOG_TYPE;

class DLL_API QLogImpl
{
public:
	QLogImpl();
public:
	static QLogImpl* GetInstance();
	void SetLogLevel(LOG_LEVEL LogLevel);
	void SetLogFilePath(LPCTSTR lpszLogFilePath);
	void WriteLog(LOG_LEVEL LogLevel, LPCTSTR lpszLogInfo);
private:
	NLock lock_;
	LOG_LEVEL	m_LogLevel;
	CString			m_strLogFilePath;
};

class DLL_API QLogHelper
{
public:
	QLogHelper(LPCSTR lpszFileName, LONG lFileLine);
	~QLogHelper();

public:
	void VLog(LPCTSTR lpszFormat,...);
	void VLog(LOG_LEVEL LogLevel, LPCTSTR lpszFormat,...);
	void VLogDbg(LPCTSTR lpszFormat, ...);
public:
	static void SetLogType(LOG_TYPE LogType);
	static void SetDbgMode(BOOL bEnable = TRUE);
private:	
	BOOL			m_bLogToFile;
	LONG			m_nFileLine;
	CString			m_strFileName;
	CString			m_strLogInfo;
	LOG_LEVEL	m_LogLevel;
	static BOOL m_bDbg;
	static LOG_TYPE m_sLogType;
};

#define QLOG_DBG(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_DBG, fmt, ##__VA_ARGS__)
#define QLOG_INFO(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define QLOG_PRO(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_PRO, fmt, ##__VA_ARGS__)
#define QLOG_APP(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_APP, fmt, ##__VA_ARGS__)
#define QLOG_WAR(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_WAR, fmt, ##__VA_ARGS__)
#define QLOG_ERR(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#define QLOG_FATAL(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#ifdef _DEBUG
#define LOG(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(fmt, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)	((void)0)
#endif

// 强制输出内容
#define LOG_PRINT(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(fmt, ##__VA_ARGS__)

// 当程序处于调试版本时，输入内容
#define LOG_PRINT_DBG(fmt, ...) QLogHelper(__FILE__, __LINE__).VLogDbg(fmt, ##__VA_ARGS__)

#ifdef _DEBUG
#define TRACE(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(fmt, ##__VA_ARGS__)
#else
#define TRACE(fmt, ...)	((void)0)
#endif