#pragma once

#include <ShellAPI.h>

#ifdef LS_STATIC_LIB_CALL
#define DLL_API
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef enum tagMSG_SHOW_LEVEL
{
	MSG_SHOW_LEVEL_ALL					= 0 ,		//	显示所有信息
	MSG_SHOW_LEVEL_IMPORTANT	= 10,	//	显示错误信息
	MSG_SHOW_LEVEL_NOTHING		= 20,	//	不显示所有信息
	MSG_SHOW_LEVEL_ALWAYS			= 30,	//	强制显示指定信息
	MSG_SHOW_LEVEL_UNDEFINED	= -1,		//	未定义
}MSG_SHOW_LEVEL;

typedef enum tagMSG_SHOW_TYPE
{
	MSG_SHOW_TYPE_ERROR			= NIIF_ERROR,
	MSG_SHOW_TYPE_INFO				= NIIF_INFO,
	MSG_SHOW_TYPE_WARNING	= NIIF_WARNING,
}MSG_SHOW_TYPE,*PMSG_SHOW_TYPE;

class DLL_API CMsgShowLevel
{
public:
	CMsgShowLevel();
	~CMsgShowLevel();

public:
	//	获取当前显示级别
	MSG_SHOW_LEVEL GetCurrentShowLevel();

	//	设置当前显示级别
	void SetCurrentShowLevel(MSG_SHOW_LEVEL MsgShowLevel);

	//	判断指定级别的消息在当前模式下是否需要显示
	BOOL IsNeedShowMsg(MSG_SHOW_LEVEL MsgLevel);

	//	设置配置信息保存路径
	void SetAppRegPath(HKEY hKey, LPCTSTR lpszRegKey);

	//	从注册表加载以前的配置信息
	MSG_SHOW_LEVEL LoadSettingFromReg();

	// 保存配置信息
	void SaveSettingsToReg();

	//	是否受服务器控制
	BOOL IsControlledByServer();

	//	设置是否受服务器控制
	void SetControlledByServer(BOOL bEnabled = TRUE);

protected:
	HKEY	m_hRegKey;				//注册表保存根键
	CString m_strRegKey;				//	注册表保存路径
	BOOL m_bAutoSaveSettings;	//	是否自动保存配置信息
	MSG_SHOW_LEVEL m_CurrentMsgShowLevel;
	BOOL m_bMsgShowLevelControlledByServer;
};