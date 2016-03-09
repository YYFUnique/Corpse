#pragma once

typedef CMap<UINT,UINT,CImageOleCtrl*,CImageOleCtrl*> COleMapTimer;

class COleTimer
{
public:
	COleTimer();
	virtual ~COleTimer();
public:
	void SetTimer(UINT nElapse);
	void KillTimer();
	static void CALLBACK TimerProc(HWND hWnd,UINT uMsg,UINT uIdEvent,DWORD dwTime);
protected:
	UINT m_nTimerID;
	static COleMapTimer m_sTimeMap;
};