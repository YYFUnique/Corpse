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
	LOG_LEVEL_ERR		=	0,
	LOG_LEVEL_WAR	= 1,
	LOG_LEVEL_APP		= 2,
	LOG_LEVEL_PRO		= 3,
	LOG_LEVEL_INFO	= 4,
	LOG_LEVEL_ALL		= 5,
}LOG_LEVEL;

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
	void VLog(LOG_LEVEL LogLevel, LPCTSTR lpszFormat,...);
	void VLog(LPCTSTR lpszFormat,...);
private:	
	BOOL			m_bLogToFile;
	LONG			m_nFileLine;
	CString			m_strFileName;
	CString			m_strLogInfo;
	LOG_LEVEL	m_LogLevel;
};

#define QLOG_INFO(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define QLOG_PRO(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_PRO, fmt, ##__VA_ARGS__)
#define QLOG_APP(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_APP, fmt, ##__VA_ARGS__)
#define QLOG_WAR(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_WAR, fmt, ##__VA_ARGS__)
#define QLOG_ERR(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)

#ifdef _DEBUG
#define LOG(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(fmt, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)	((void)0)
#endif

#define LOG_PRINT(fmt, ...) QLogHelper(__FILE__, __LINE__).VLog(fmt, ##__VA_ARGS__)