#pragma once

class IVirtualWndMgr
{
public:
	IVirtualWndMgr();
	virtual ~IVirtualWndMgr();
public:
	virtual void ClearVirtualWnd(CNotifyPump* pNotifyPump) = 0;
	virtual void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager) = 0;
};