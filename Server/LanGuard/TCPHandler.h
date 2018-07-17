#pragma once

#include "ITCPHandler.h"
#include "ADELLock.h"
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
	CADELLock* m_pADELLock;
};