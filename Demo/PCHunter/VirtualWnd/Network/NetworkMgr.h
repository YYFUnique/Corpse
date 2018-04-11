#pragma once

#include "Route.h"

class CNetworkMgr : public CNotifyPump,public IVirtualWndMgr
{
public:
	CNetworkMgr();
	~CNetworkMgr();

public:
	void ClearVirtualWnd(CNotifyPump* pNotifyPump);
	void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnSelectChanged( TNotifyUI &msg );
private:
	CRoute		m_Route;
	CPaintManagerUI* m_pPaintManager;
};