#pragma once

#include "libcurl/libcurl.h"

#define HTTP_GET_CITY_LOCATION		0x1000		//ͨ���ٶ�API��ȡ������
#define HTTP_GET_CITY_WEATHER		0x1001		//��ȡ��������

class CWeatherHelper : public ILibcurlCallback
{
public:
	CWeatherHelper();
	~CWeatherHelper();

public:
	//BOOL GetCityLocation();
protected:
	//��ȡ������
	void OnGetCityLocation(LPVOID lpData);
	//��ȡ��������
	void OnGetCityWeather(LPVOID lpData);
	//����HTTP���󷵻�����
	int Progress(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb);

private:
	CLibcurl*				m_pLibcurl;
};