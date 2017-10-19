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
	//���ó���λ����Ϣ
	void SetCityLocation(LPCTSTR lpszCityLocate);
	BOOL GetCityLocation(CString& strCityLocation);

	//���ó���������Ϣ����С�꣬С��ת�����
	void SetCityWeatherInfo(LPCTSTR lpszCityWeatherInfo);
	BOOL GetCityWeatherInfo(CString& strCityWeatherInfo);

	//���ó����¶���Ϣ
	void SetCityTemperature(LPCTSTR lpszCityTemperature);
	BOOL GetCityTemperature(CString& strCityTemperature);

	//���ó����¶ȱ仯��Χ
	void SetCityTemperatureRange(LPCTSTR lpszCityTemperatureRange);
	BOOL GetCityTemperatureRange(CString& strCityTemperatureRange);

	//���ó���PM2.5��Ϣ
	void SetCityPM25Info(LPCTSTR lpszCityPM25Info);
	BOOL GetCityPM25Info(CString& strCityPM25Info);

	//���ó��п�������������Ϣ
	void SetCityPM25Remark(LPCTSTR lpszCityPM25Remark);
	BOOL GetCityPM25Remark(CString& strCityPM25Remark);
private:
	CJsonObject			m_JsonObject;
	CityInfoObserver		m_CityInfoObserver;
};