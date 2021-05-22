#include "StdAfx.h"
#include "TerminalData.h"

#include "DllCore/Json/JsonObject.h"

#define TERMINAL_RECV_VER		_T("version")
#define TERMINAL_RECV_CMD		_T("cmd")
#define TERMINAL_RECV_DATA	_T("data")
CTerminalData::CTerminalData(LPCTSTR lpszTerminal)
	: m_nVer(1)
	, m_ADELCmd(ADEL_LOCK_CMD_DEFAULT)
{
	CJsonObject Json;
	Json.FromString(lpszTerminal);

	if (Json.IsMember(TERMINAL_RECV_VER) != FALSE)
		Json.GetValue(TERMINAL_RECV_VER, &m_nVer);
	if (Json.IsMember(TERMINAL_RECV_CMD) != FALSE)
		Json.GetValue(TERMINAL_RECV_CMD, (int*)&m_ADELCmd);
	if (Json.IsMember(TERMINAL_RECV_DATA) != FALSE)
		Json.GetValue(TERMINAL_RECV_DATA, &m_JsonSubData);
}

CTerminalData::~CTerminalData()
{

}

int CTerminalData::GetProtocol() const
{
	return m_nVer;
}

ADEL_LOCK_CMD CTerminalData::GetADELCmd() const
{
	return m_ADELCmd;
}

CString CTerminalData::GetSubData() const
{
	return m_JsonSubData.ToString();
}