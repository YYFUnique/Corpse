#pragma once

#include "ChangeNotify.h"
#include "DllCore/Json/JsonObject.h"
#include "DuiLib/Control/UIMenu.h"

class CCityHelper
{
public:
	CCityHelper();
	~CCityHelper();
	void Init(CityInfoReceiver* pReceiver);
	void LoadData();
public:
	void SetCityLocation(LPCTSTR lpszCityLocate);
	BOOL GetCityLocation(CString& strCityLocation);

	void SetCityTemperature(LPCTSTR lpszCityTemperature);
	BOOL GetCityTemperature(CString& strCityTemperature);

	void SetCityTemperatureRange(LPCTSTR lpszCityTemperatureRange);
	BOOL GetCityTemperatureRange(CString& strCityTemperatureRange);
private:
	CJsonObject			m_JsonObject;
	CityInfoObserver		m_CityInfoObserver;
};