#pragma once
#include "CityHelper.h"

class CCityWeatherInfo : public WindowImplBase
{
public:
	CCityWeatherInfo(HWND hParent, CCityHelper* pCityInfo);
	~CCityWeatherInfo();
public:
	void OnFinalMessage( HWND hWnd );
	virtual LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	UILIB_RESOURCETYPE GetResourceType() const;
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
protected:
	void OnTimer(TNotifyUI& msg);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
private:
	HWND				m_hParent;
	CCityHelper*	m_pCityInfo;
};