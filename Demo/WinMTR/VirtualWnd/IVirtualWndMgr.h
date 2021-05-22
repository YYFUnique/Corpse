#pragma once

class IVirtualWndMgr
{
public:
	IVirtualWndMgr();
	virtual ~IVirtualWndMgr();
public:
	virtual void NotifyTask(PCNTCHDR pNTCHDR);
	virtual void ClearVirtualWnd(CNotifyPump* pNotifyPump) = 0;
	virtual void AddVirtualWnd(LPCTSTR lpszVirtualWnd) = 0 ;
	virtual void SetVirtualWnd(CNotifyPump* pNotifyPump, CPaintManagerUI* pPaintManager) = 0;
};