#include "stdafx.h"
#include "OleTimer.h"

COleMapTimer COleTimer::m_sTimeMap;

COleTimer::COleTimer()
:m_nTimerID(0)
{

}

COleTimer::~COleTimer()
{

}

void COleTimer::SetTimer(UINT nElapse,CImageOleCtrl* pImageOleCtrl)
{
	KillTimer();
	m_nTimerID = SetTimer(NULL,NULL,nElapse,TimerProc);
	m_sTimeMap[nElapse] = pImageOleCtrl;
}

void COleTimer::KillTimer()
{
	if (m_nTimerID != 0)
	{
		::KillTimer(NULL,m_nTimerID);
		m_sTimeMap.RemoveKey(m_nTimerID);
	}
}

void CALLBACK COleTimer::TimerProc(HWND hWnd,UINT uMsg,UINT uIdEvent,DWORD dwTime)
{
	RECT rcImageOlePos;
	m_sTimeMap[uIdEvent]->GetOleRect(&rcImageOlePos);
}