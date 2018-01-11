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
	MSG_SHOW_LEVEL_ALL					= 0 ,		//	��ʾ������Ϣ
	MSG_SHOW_LEVEL_IMPORTANT	= 10,	//	��ʾ������Ϣ
	MSG_SHOW_LEVEL_NOTHING		= 20,	//	����ʾ������Ϣ
	MSG_SHOW_LEVEL_ALWAYS			= 30,	//	ǿ����ʾָ����Ϣ
	MSG_SHOW_LEVEL_UNDEFINED	= -1,		//	δ����
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
	//	��ȡ��ǰ��ʾ����
	MSG_SHOW_LEVEL GetCurrentShowLevel();

	//	���õ�ǰ��ʾ����
	void SetCurrentShowLevel(MSG_SHOW_LEVEL MsgShowLevel);

	//	�ж�ָ���������Ϣ�ڵ�ǰģʽ���Ƿ���Ҫ��ʾ
	BOOL IsNeedShowMsg(MSG_SHOW_LEVEL MsgLevel);

	//	����������Ϣ����·��
	void SetAppRegPath(HKEY hKey, LPCTSTR lpszRegKey);

	//	��ע��������ǰ��������Ϣ
	MSG_SHOW_LEVEL LoadSettingFromReg();

	// ����������Ϣ
	void SaveSettingsToReg();

	//	�Ƿ��ܷ���������
	BOOL IsControlledByServer();

	//	�����Ƿ��ܷ���������
	void SetControlledByServer(BOOL bEnabled = TRUE);

protected:
	HKEY	m_hRegKey;				//ע��������
	CString m_strRegKey;				//	ע�����·��
	BOOL m_bAutoSaveSettings;	//	�Ƿ��Զ�����������Ϣ
	MSG_SHOW_LEVEL m_CurrentMsgShowLevel;
	BOOL m_bMsgShowLevelControlledByServer;
};