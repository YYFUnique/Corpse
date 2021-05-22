#pragma once

#include "ITCPHandler.h"
#include "ADELLock.h"
#include "TerminalData.h"

class CTCPHandler : public ITCPHandler
{
public:
	CTCPHandler();
	~CTCPHandler();

public:
	void Init();
	virtual void OnClose();
	virtual void RecvRoute(int nClientId, LPCSTR lpszData, int nLen);
protected:
	void OnRecvData(const CTerminalData& TerminalData, CJsonObject& JsonData);
	int OnADELLockInit(LPCTSTR lpszInitData);
	int OnADELLockEnd();
	int OnADELLockChangeUser(LPCTSTR lpszChangeData);
	int OnADELLockNewKey(LPCTSTR lpszNewData);
	int OnADELLockEraseCard(LPCTSTR lpszEraseData, CJsonObject& JsonData);
	int OnADELLockReadCard(CJsonObject& JsonReadCard);
	int OnADELLockReadCardId(CJsonObject& JsonReadCardId);
protected:
	CADELLock* m_pADELLock;
};