#pragma once

class IVirtualWndMgr
{
public:
	IVirtualWndMgr();
	virtual ~IVirtualWndMgr();
public:
	virtual void NotifyTask(PCNTCHDR pNTCHDR);
	virtual void ClearVirtualWnd(CNotifyPump* pNotifyPump) = 0;
	virtual void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager) = 0;
	virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};