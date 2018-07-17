#pragma once

typedef enum tagDEBUG_TYPE
{
	DEBUG_TYPE_NONE		= 0,	//	无调试信息输出
	DEBUG_TYPE_OUTPUT	=	1,	// 调试信息输出到DbView
	DEBUG_TYPE_FILE			=	2,	//	调试信息输出到文件
}DEBUG_TYPE;

class CLanGuardConfig
{
public:
	CLanGuardConfig();
	~CLanGuardConfig();

public:
	BOOL Load(LPCTSTR lpszConfigPath);
	WORD GetPort() const;
	DEBUG_TYPE GetDebugType() const;

protected:
	WORD				m_wPort;
	DEBUG_TYPE	m_DbType;
};