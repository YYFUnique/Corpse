#pragma once

#include "CityHelper.h"
#include "libcurl/libcurl.h"

#define HTTP_GET_CITY_LOCATION		0x1000		//通过百度API获取城市名
#define HTTP_GET_CITY_WEATHER		0x1001		//获取城市天气

class CWeatherHelper : public ILibcurlCallback
{
public:
	CWeatherHelper();
	~CWeatherHelper();

public:
	void Init(CCityHelper* pCityHelper);
	BOOL GetCityLocation();
	BOOL GetCityWeather(LPCTSTR lpszCityInfo);
protected:
	//获取城市名
	void OnGetCityLocation(LPVOID lpData);
	//获取城市天气
	void OnGetCityWeather(LPVOID lpData);
	//处理HTTP请求返回内容
	int Progress(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb);

private:
	CLibcurl*				m_pLibcurl;
	CCityHelper*		m_pCityHelper;
};