#include "StdAfx.h"
#include "CityHelper.h"
#include <atlstr.h>

//用于存放城市天气信息JSON结构字段
#define JSON_MEMBER_CITY_LOCATION							_T("citylocation")
#define JSON_MEMBER_CITY_WEATHERINFO					_T("weatherinfo")
#define JSON_MEMBER_CITY_TEMPERATURE					_T("temperature")
#define JSON_MEMBER_CITY_TEMPERATURERANGE		_T("temperaturerange")
#define JSON_MEMBER_CITY_PM25INFO							_T("pm25info")
#define JSON_MEMBER_CITY_PM25INFO_REMARK			_T("pm25_remark")

CCityHelper::CCityHelper()
{

}

CCityHelper::~CCityHelper()
{

}

void CCityHelper::Init(CityInfoReceiver* pReceiver)
{
	m_CityInfoObserver.AddReceiver(pReceiver);
}

void CCityHelper::LoadData()
{
	CityInfoChangedParam CityInfoParam;
	CityInfoParam.CityInfoChanged = CITYINFO_CHANGED_LOAD_SUCCESS;
	CityInfoParam.strJsonString = _T("");
	m_CityInfoObserver.Broadcast(CityInfoParam);
}

void CCityHelper::SetCityLocation(LPCTSTR lpszCityLocate)
{
	m_JsonObject.SetValue(JSON_MEMBER_CITY_LOCATION, lpszCityLocate);
	CityInfoChangedParam CityInfoParam;
	CityInfoParam.CityInfoChanged = CITYINFO_CHANGED_LOCATION;
	CityInfoParam.strJsonString = lpszCityLocate;
	m_CityInfoObserver.Broadcast(CityInfoParam);
}

BOOL CCityHelper::GetCityLocation(CString& strCityLocation)
{
	strCityLocation.Empty();

	if (m_JsonObject.IsMember(JSON_MEMBER_CITY_LOCATION))
		strCityLocation = m_JsonObject.GetValue(JSON_MEMBER_CITY_LOCATION);

	return strCityLocation.IsEmpty() == FALSE;
}

void CCityHelper::SetCityWeatherInfo(LPCTSTR lpszCityWeatherInfo)
{
	m_JsonObject.SetValue(JSON_MEMBER_CITY_WEATHERINFO, lpszCityWeatherInfo);

	/*CityInfoChangedParam CityInfoParam;
	CityInfoParam.CityInfoChanged = CITYINFO_CHANGED_WEATHERINFO;
	CityInfoParam.strJsonString = lpszCityWeatherInfo;
	m_CityInfoObserver.Broadcast(CityInfoParam);*/
}

BOOL CCityHelper::GetCityWeatherInfo(CString& strCityWeatherInfo)
{
	strCityWeatherInfo.Empty();

	if (m_JsonObject.IsMember(JSON_MEMBER_CITY_WEATHERINFO))
		strCityWeatherInfo = m_JsonObject.GetValue(JSON_MEMBER_CITY_WEATHERINFO);

	return strCityWeatherInfo.IsEmpty() == FALSE;
}

void CCityHelper::SetCityTemperature(LPCTSTR lpszCityTemperature)
{
	m_JsonObject.SetValue(JSON_MEMBER_CITY_TEMPERATURE, lpszCityTemperature);

	CityInfoChangedParam CityInfoParam;
	CityInfoParam.CityInfoChanged = CITYINFO_CHANGED_TEMPERATURE;
	CityInfoParam.strJsonString = lpszCityTemperature;
	m_CityInfoObserver.Broadcast(CityInfoParam);
}

BOOL CCityHelper::GetCityTemperature(CString& strCityTemperature)
{
	strCityTemperature.Empty();
	
	if (m_JsonObject.IsMember(JSON_MEMBER_CITY_TEMPERATURE))
		strCityTemperature = m_JsonObject.GetValue(JSON_MEMBER_CITY_TEMPERATURE);

	return strCityTemperature.IsEmpty() == FALSE;
}

void CCityHelper::SetCityTemperatureRange(LPCTSTR lpszCityTemperatureRange)
{
	m_JsonObject.SetValue(JSON_MEMBER_CITY_TEMPERATURERANGE, lpszCityTemperatureRange);
}

BOOL CCityHelper::GetCityTemperatureRange(CString& strCityTemperatureRange)
{
	strCityTemperatureRange.Empty();

	if (m_JsonObject.IsMember(JSON_MEMBER_CITY_TEMPERATURERANGE))
		strCityTemperatureRange = m_JsonObject.GetValue(JSON_MEMBER_CITY_TEMPERATURERANGE);

	return strCityTemperatureRange.IsEmpty() == FALSE;
}

void CCityHelper::SetCityPM25Info(LPCTSTR lpszCityPM25Info)
{
	m_JsonObject.SetValue(JSON_MEMBER_CITY_PM25INFO, lpszCityPM25Info);
}

BOOL CCityHelper::GetCityPM25Info(CString& strCityPM25Info)
{
	strCityPM25Info.Empty();

	if (m_JsonObject.IsMember(JSON_MEMBER_CITY_PM25INFO))
		strCityPM25Info = m_JsonObject.GetValue(JSON_MEMBER_CITY_PM25INFO);

	return strCityPM25Info.IsEmpty() == FALSE;
}

void CCityHelper::SetCityPM25Remark(LPCTSTR lpszCityPM25Remark)
{
	m_JsonObject.SetValue(JSON_MEMBER_CITY_PM25INFO_REMARK, lpszCityPM25Remark);
}

BOOL CCityHelper::GetCityPM25Remark(CString& strCityPM25Remark)
{
	strCityPM25Remark.Empty();

	if (m_JsonObject.IsMember(JSON_MEMBER_CITY_PM25INFO_REMARK))
		strCityPM25Remark = m_JsonObject.GetValue(JSON_MEMBER_CITY_PM25INFO_REMARK);

	return strCityPM25Remark.IsEmpty() == FALSE;
}