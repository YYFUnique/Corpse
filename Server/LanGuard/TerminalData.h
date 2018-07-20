#pragma once
#include "ADELLock.h"
#include "DllCore/Json/JsonObject.h"
class CTerminalData
{
public:
	CTerminalData(LPCTSTR lpszTerminal);
	~CTerminalData();

public:
	int GetProtocol() const;
	ADEL_LOCK_CMD GetADELCmd() const;
	CString GetSubData() const;
protected:
	int							m_nVer;
	CJsonObject			m_JsonSubData;
	ADEL_LOCK_CMD	m_ADELCmd;
};