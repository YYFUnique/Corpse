#include "StdAfx.h"
#include "CityHelper.h"
#include <atlstr.h>

#define JSON_MEMBER_CITY_LOCATION							_T("citylocation")
#define JSON_MEMBER_CITY_TEMPERATURE					_T("temperature")
#define JSON_MEMBER_CITY_TEMPERATURERANGE		_T("temperaturerange")

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

void CCityHelper::SetCityTemperature(LPCTSTR lpszCityTemperature)
{
	m_JsonObject.SetValue(JSON_MEMBER_CITY_TEMPERATURE, lpszCityTemperature);

	CityInfoChangedParam CityInfoParam;
	CityInfoParam.CityInfoChanged = CITYINFO_CHANGED_WEATHER;
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