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
	//设置城市位置信息
	void SetCityLocation(LPCTSTR lpszCityLocate);
	BOOL GetCityLocation(CString& strCityLocation);

	//设置城市天气信息，如小雨，小雨转中雨等
	void SetCityWeatherInfo(LPCTSTR lpszCityWeatherInfo);
	BOOL GetCityWeatherInfo(CString& strCityWeatherInfo);

	//设置城市温度信息
	void SetCityTemperature(LPCTSTR lpszCityTemperature);
	BOOL GetCityTemperature(CString& strCityTemperature);

	//设置城市温度变化范围
	void SetCityTemperatureRange(LPCTSTR lpszCityTemperatureRange);
	BOOL GetCityTemperatureRange(CString& strCityTemperatureRange);

	//设置城市PM2.5信息
	void SetCityPM25Info(LPCTSTR lpszCityPM25Info);
	BOOL GetCityPM25Info(CString& strCityPM25Info);

	//设置城市空气质量描述信息
	void SetCityPM25Remark(LPCTSTR lpszCityPM25Remark);
	BOOL GetCityPM25Remark(CString& strCityPM25Remark);
private:
	CJsonObject			m_JsonObject;
	CityInfoObserver		m_CityInfoObserver;
};