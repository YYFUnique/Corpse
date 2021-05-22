#include "StdAfx.h"
#include "AutoLog.h"
#include "LogHelper.h"

CAutoLog::CAutoLog(LPCTSTR lpszModuleName, LPCTSTR lpszFuncName)
{
	m_strModuleName = lpszModuleName;
	m_strFuncName = lpszFuncName;

	QLOG_INFO(_T("[%s]-----Enter----->%s.\n"), m_strModuleName, m_strFuncName);
}

CAutoLog::~CAutoLog()
{
	QLOG_INFO(_T("[%s]-----Leave----->%s.\n"), m_strModuleName, m_strFuncName);
}