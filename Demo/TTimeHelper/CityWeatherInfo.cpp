#include "stdafx.h"
#include "CityWeatherInfo.h"

#define CITY_WEATHER_INFO_TIMED_ID		0x1001

CCityWeatherInfo::CCityWeatherInfo(HWND hParent)
{
	m_hParent = hParent;
	Create(hParent, _T("CityWeatherInfo"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CCityWeatherInfo::~CCityWeatherInfo()
{

}

void CCityWeatherInfo::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CCityWeatherInfo::GetWindowClassName() const
{
	return _T("CityWeatherInfo");
}

CDuiString CCityWeatherInfo::GetSkinFile()
{
	return _T("WeatherInfo.xml");
}

CDuiString CCityWeatherInfo::GetZIPFileName() const
{
	return _T("");
}

CDuiString CCityWeatherInfo::GetSkinFolder()
{
	return _T("TTimerHelper");
}

UILIB_RESOURCETYPE CCityWeatherInfo::GetResourceType() const
{
	return UILIB_FILE;
}

void CCityWeatherInfo::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_TIMER)
		OnTimer(msg);
}

void CCityWeatherInfo::InitWindow()
{
	CControlUI* pLayout = m_PaintManager.FindControl(_T("HLayoutTotal"));
	if (pLayout != NULL)
		m_PaintManager.SetTimer(pLayout, CITY_WEATHER_INFO_TIMED_ID, 500);

	RECT rcWnd;
	GetWindowRect(m_hParent, &rcWnd);

	SetWindowPos(m_hWnd, HWND_BOTTOM, rcWnd.left, rcWnd.bottom, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);
}

LRESULT CCityWeatherInfo::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	Close(IDOK);

	return 0;
}

void CCityWeatherInfo::OnTimer(TNotifyUI& msg)
{
	if (msg.wParam == CITY_WEATHER_INFO_TIMED_ID)
	{
		POINT pt;
		GetCursorPos(&pt);
		//���ڸ����ڲ��Ҳ��ڱ�����
		HWND hWnd = m_hWnd;
		do 
		{
			RECT rcWnd;
			GetWindowRect(hWnd, &rcWnd);
			if (::PtInRect(&rcWnd, pt))
				break;

			//������Ѿ�ѭ�����������ˣ�����û�м�⵽�����ָ�������ڣ�����Ϊ�����ָ��������
			if (hWnd == m_hParent)
			{
				Close(IDCANCEL);
				m_PaintManager.KillTimer(msg.pSender);
				break;
			}

			hWnd = GetParent(hWnd);
		} while (hWnd != NULL);
	}
}