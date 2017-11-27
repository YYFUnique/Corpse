#pragma once

#include "CityHelper.h"
#include "libcurl/libcurl.h"
#include "libcurl/MultiRequest.h"

#define HTTP_GET_CITY_LOCATION		0x1000		//通过百度API获取城市名
#define HTTP_GET_CITY_WEATHER		0x1001		//获取城市天气
#define HTTP_GET_CITY_AQIINFO			0x1002		//获取城市空气质量
class CWeatherHelper : public ILibcurlCallback
{
public:
	CWeatherHelper();
	~CWeatherHelper();

public:
	void Init(CCityHelper* pCityHelper);
	BOOL GetCityLocation();
	BOOL GetCityWeather(LPCTSTR lpszCityInfo);
	BOOL GetCityAQIInfo(LPCTSTR lpszCityInfo);
protected:
	//获取城市名
	void OnGetCityLocation(LPVOID lpData);
	//获取城市天气
	void OnGetCityWeather(LPVOID lpData);
	//获取城市空气质量
	void OnGetCityPM25Info(LPVOID lpData);
	//处理HTTP请求返回内容
	int ProcessFunc(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb);
	//当前HTTP处理结果，一个请求可能会出现多个结果
	void LibcurlNotify(DWORD dwEvent, CURLcode curlCode, LPVOID lpData);
private:
	CMultiRequest*	m_pMulti;
	CCityHelper*		m_pCityHelper;
};