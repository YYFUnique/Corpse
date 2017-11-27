#pragma once

#include "CityHelper.h"
#include "libcurl/libcurl.h"
#include "libcurl/MultiRequest.h"

#define HTTP_GET_CITY_LOCATION		0x1000		//ͨ���ٶ�API��ȡ������
#define HTTP_GET_CITY_WEATHER		0x1001		//��ȡ��������
#define HTTP_GET_CITY_AQIINFO			0x1002		//��ȡ���п�������
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
	//��ȡ������
	void OnGetCityLocation(LPVOID lpData);
	//��ȡ��������
	void OnGetCityWeather(LPVOID lpData);
	//��ȡ���п�������
	void OnGetCityPM25Info(LPVOID lpData);
	//����HTTP���󷵻�����
	int ProcessFunc(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb);
	//��ǰHTTP��������һ��������ܻ���ֶ�����
	void LibcurlNotify(DWORD dwEvent, CURLcode curlCode, LPVOID lpData);
private:
	CMultiRequest*	m_pMulti;
	CCityHelper*		m_pCityHelper;
};