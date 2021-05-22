#include "StdAfx.h"
#include "LanGuardConfig.h"
#include "DllCore/Utils/ErrorInfo.h"

#define APP_CONFIG					_T("Server")		//	配置文件中APP名称
#define KEY_PORT_CONFIG		_T("Port")			//	配置文件中端口号
#define KEY_DEBUG_CONFIG		_T("Debug")		//	配置文件中调试方式


#define DEFAULT_SERVER_PORT		6611						//	服务器默认端口

#define DEFAULT_DEBUG_TYPE		DEBUG_TYPE_NONE	//	服务器调试方式

CLanGuardConfig::CLanGuardConfig()
	: m_wPort(DEFAULT_SERVER_PORT)
	, m_DbType(DEFAULT_DEBUG_TYPE)
{

}

CLanGuardConfig::~CLanGuardConfig()
{

}

BOOL CLanGuardConfig::Load(LPCTSTR lpszConfigPath)
{
	BOOL bSuccess = FALSE;
	do 
	{
		// 获取当前执行程序路径
 		TCHAR szModuleFilePath[MAX_PATH];
 		GetModuleFileName(NULL, szModuleFilePath, _countof(szModuleFilePath));
 		PathRemoveFileSpec(szModuleFilePath);
		// 拼接配置文件路径
		PathAppend(szModuleFilePath, lpszConfigPath);

		m_wPort = GetPrivateProfileInt(APP_CONFIG, KEY_PORT_CONFIG, DEFAULT_SERVER_PORT, szModuleFilePath);

		m_DbType = (DEBUG_TYPE)GetPrivateProfileInt(APP_CONFIG, KEY_DEBUG_CONFIG, DEFAULT_DEBUG_TYPE, szModuleFilePath);

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

WORD CLanGuardConfig::GetPort() const
{
	return m_wPort;
}

DEBUG_TYPE CLanGuardConfig::GetDebugType() const
{
	return m_DbType;
}