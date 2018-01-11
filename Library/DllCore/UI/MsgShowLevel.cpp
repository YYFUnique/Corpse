#include "StdAfx.h"
#include "MsgShowLevel.h"

//	消息提示级别
#define MSG_SHOW_LEVEL_INFO											_T("MsgShowLevel")
//	消息是否受服务器控制
#define MSG_SHOW_LEVEL_CONTROLLED_BY_SERVER		_T("MsgShowLevelControlledByServer")

CMsgShowLevel::CMsgShowLevel()
{
	m_bMsgShowLevelControlledByServer = TRUE;
	m_CurrentMsgShowLevel = MSG_SHOW_LEVEL_IMPORTANT;
	m_bAutoSaveSettings = TRUE;
	m_hRegKey = NULL;
}

CMsgShowLevel::~CMsgShowLevel()
{

}

void CMsgShowLevel::SetAppRegPath(HKEY hKey, LPCTSTR lpszRegKey)
{
	m_hRegKey = hKey;
	m_strRegKey = lpszRegKey;
}

MSG_SHOW_LEVEL CMsgShowLevel::LoadSettingFromReg()
{
	HKEY hKey = NULL;
	do 
	{
		if (m_hRegKey == NULL)
			break;

		if (RegOpenKeyEx(m_hRegKey, m_strRegKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
			break;

		DWORD dwValue, dwSize = sizeof(DWORD);
		if (RegQueryValueEx(hKey, MSG_SHOW_LEVEL_INFO, NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
			m_CurrentMsgShowLevel = (MSG_SHOW_LEVEL)dwValue;

		if (RegQueryValueEx(hKey, MSG_SHOW_LEVEL_CONTROLLED_BY_SERVER, NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
			m_bMsgShowLevelControlledByServer = dwValue;

		break;
	} while (FALSE);

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}

	return m_CurrentMsgShowLevel;
}

void CMsgShowLevel::SaveSettingsToReg()
{
	if (m_bAutoSaveSettings == FALSE || m_hRegKey == NULL || m_strRegKey.IsEmpty())
		return;

	HKEY hKey = NULL;
	do 
	{
		if (RegOpenKeyEx(m_hRegKey, m_strRegKey,0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
			break;

		DWORD dwSize = sizeof(DWORD);
		RegSetValueEx(hKey, MSG_SHOW_LEVEL_INFO, 0, 0, (LPBYTE)&m_CurrentMsgShowLevel, &dwSize);
		RegSetValueEx(hKey, MSG_SHOW_LEVEL_CONTROLLED_BY_SERVER, 0, 0, (LPBYTE)&m_bMsgShowLevelControlledByServer, &dwSize);
	} while (FALSE);

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
		hKey = NULL;
	}
}

MSG_SHOW_LEVEL CMsgShowLevel::GetCurrentShowLevel()
{
	return m_CurrentMsgShowLevel;
}

void CMsgShowLevel::SetCurrentShowLevel(MSG_SHOW_LEVEL MsgShowLevel)
{
	if (m_CurrentMsgShowLevel == MsgShowLevel)
		return;

	m_CurrentMsgShowLevel = MsgShowLevel;
	SaveSettingsToReg();
}

BOOL CMsgShowLevel::IsNeedShowMsg(MSG_SHOW_LEVEL MsgLevel)
{
	return MsgLevel >= m_CurrentMsgShowLevel;
}

BOOL CMsgShowLevel::IsControlledByServer()
{
	return m_bMsgShowLevelControlledByServer;
}

void CMsgShowLevel::SetControlledByServer(BOOL bEnabled /* = TRUE */)
{
	if (m_bMsgShowLevelControlledByServer == bEnabled)
		return;

	m_bMsgShowLevelControlledByServer = bEnabled;
	SaveSettingsToReg();
}