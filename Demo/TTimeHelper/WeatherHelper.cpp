#include "StdAfx.h"
#include "WeatherHelper.h"
#include "DllCore/Utils/StdPtr.h"
#include "DllCore/Utils/TextTools.h"

CWeatherHelper::CWeatherHelper()
{
	m_pMulti = NULL;
}

CWeatherHelper::~CWeatherHelper()
{
	if (m_pMulti != NULL)
	{
		delete m_pMulti;
		m_pMulti = NULL;
	}
}

void CWeatherHelper::Init(CCityHelper* pCityHelper)
{
	m_pMulti = new CMultiRequest;

	if (m_pMulti)
	{
		m_pMulti->Init();
		m_pMulti->SetCallback(this);
	}

	//设置全局城市信息保存指针
	if (pCityHelper != NULL)
		m_pCityHelper = pCityHelper;
}

BOOL CWeatherHelper::GetCityLocation()
{
	LPCTSTR lpszAk = _T("tx0s5wZjHvEunl28nOu1a2s4HbRfZvy6");
	LPCTSTR lpszBaiDuAPI = _T("http://api.map.baidu.com/location/ip?ak=%s");
	CDuiString strCityLocation;
	strCityLocation.Format(lpszBaiDuAPI, lpszAk);
	if (m_pMulti == NULL)
		return FALSE;

	return m_pMulti->doHttpGet(HTTP_GET_CITY_LOCATION, strCityLocation);
}

BOOL CWeatherHelper::GetCityWeather(LPCTSTR lpszCityInfo)
{
	if (lpszCityInfo == NULL || _tcslen(lpszCityInfo) == 0)
		return FALSE;
	//http://api.k780.com/?app=weather.today&weaid=1&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json
	LPCTSTR lpszAppKey = _T("25122");
	LPCTSTR lpszSign = _T("94eaf1a9e74eddd9357947078458fa11");
	CString strCityTemperature;
	//http://api.k780.com/?app=weather.today&weaid=1&appkey=25122&sign=94eaf1a9e74eddd9357947078458fa11&format=json
	// 接口更新地址： https://www.nowapi.com/
	strCityTemperature.Format(_T("http://api.k780.com/?app=weather.today&weaid=%s&appkey=%s&sign=%s&format=json"),
		lpszCityInfo, lpszAppKey, lpszSign);

	if (m_pMulti == NULL)
		return FALSE;

	CString strEncodeURL;

	URLEncode(strCityTemperature, strEncodeURL);

	return m_pMulti->doHttpGet(HTTP_GET_CITY_WEATHER, strEncodeURL);
}

BOOL CWeatherHelper::GetCityAQIInfo(LPCTSTR lpszCityInfo)
{
	if (lpszCityInfo == NULL || _tcslen(lpszCityInfo) == 0)
		return FALSE;
	//http://api.k780.com/?app=weather.today&weaid=1&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json
	LPCTSTR lpszAppKey = _T("25122");
	LPCTSTR lpszSign = _T("94eaf1a9e74eddd9357947078458fa11");
	CString strCityTemperature;
	//http://api.k780.com/?app=weather.today&weaid=1&appkey=25122&sign=94eaf1a9e74eddd9357947078458fa11&format=json
	strCityTemperature.Format(_T("http://api.k780.com/?app=weather.pm25&weaid=%s&appkey=%s&sign=%s&format=json"),
		lpszCityInfo, lpszAppKey, lpszSign);

	if (m_pMulti == NULL)
		return FALSE;

	CString strEncodeURL;

	URLEncode(strCityTemperature, strEncodeURL);

	return m_pMulti->doHttpGet(HTTP_GET_CITY_AQIINFO, strEncodeURL);
}

int CWeatherHelper::ProcessFunc(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb)
{
	if (dwEvent == HTTP_GET_CITY_LOCATION)
		OnGetCityLocation(lpData);
	else if (dwEvent == HTTP_GET_CITY_WEATHER)
		OnGetCityWeather(lpData);
	else if (dwEvent == HTTP_GET_CITY_AQIINFO)
		OnGetCityPM25Info(lpData);
	return size * nmemb;
}

void CWeatherHelper::LibcurlNotify(DWORD dwEvent, CURLcode curlCode, LPVOID lpData)
{
	//如果辅助类不需要进行处理，可以将结果传递给调用程序，由调用程序处理
	if (dwEvent == HTTP_GET_CITY_LOCATION)
	{
		if (curlCode == CURLE_COULDNT_CONNECT)
			OutputDebugString(_T("网络连接已断开"));
		else if (curlCode == CURLE_COULDNT_RESOLVE_HOST)
			OutputDebugString(_T("无法解析DNS域名"));
	}
	return;
}

void CWeatherHelper::OnGetCityLocation(LPVOID lpData)
{
	CString strWeatherInfo((LPCSTR)lpData);

	CJsonObject JsonObject;
	JsonObject.FromString(strWeatherInfo);

	int nErrorNo = -1;

	if (JsonObject.IsMember(_T("status")))
		JsonObject.GetValue(_T("status"), &nErrorNo);

	if (nErrorNo != 0)
		return;

	CString strAddr;
	if (JsonObject.IsMember(_T("address")))
	{
		CStdArray strAddInfo;
		strAddr = JsonObject.GetValue(_T("address"));
		AnalyseFormattedTextInfoItem(strAddr, strAddInfo, _T('|'));

		//将位置信息保存到全局CityHelper类中
		if (m_pCityHelper != NULL)
			m_pCityHelper->SetCityLocation(strAddInfo.GetAt(2));
	}
}

void CWeatherHelper::OnGetCityWeather(LPVOID lpData)
{
	LPWSTR lpwText = NULL;
	DWORD dwSize = 0;
	if (QXUtf8ToUnicode((LPCSTR)lpData, lpwText, dwSize) == FALSE)
		lpwText = new WCHAR[dwSize];
	QXUtf8ToUnicode((LPCSTR)lpData, lpwText, dwSize);

	CString strCityWeatherInfo(lpwText);
	if (lpwText)
	{
		delete[] lpwText;
		lpwText = NULL;
	}
	
	CJsonObject JsonObject;
	JsonObject.FromString(strCityWeatherInfo);

	CString strErrorNo;

	if (JsonObject.IsMember(_T("success")))
		strErrorNo = JsonObject.GetValue(_T("success"));

	//接口获取数据成功返回1
	if (strErrorNo.CompareNoCase(_T("1")) != 0)
		return;

	CJsonObject JsonObjectResult;

	if (JsonObject.IsMember(_T("result")))
		JsonObject.GetValue(_T("result"), &JsonObjectResult);

	CString strTemperature;
	CString strTemperatureRange;
	if (JsonObjectResult.IsMember(_T("temperature")))
		strTemperatureRange = JsonObjectResult.GetValue(_T("temperature"));
	if (JsonObjectResult.IsMember(_T("temperature")))
		strTemperature = JsonObjectResult.GetValue(_T("temperature_curr"));

	CString strWeatherInfo;
	if (JsonObjectResult.IsMember(_T("weather_curr")))
		strWeatherInfo = JsonObjectResult.GetValue(_T("weather_curr"));

	if (m_pCityHelper != NULL)
	{
		//设置城市当前温度
		m_pCityHelper->SetCityTemperature(strTemperature);
		//设置城市温度变化范围
		m_pCityHelper->SetCityTemperatureRange(strTemperatureRange);
		//设置城市当前天气信息
		m_pCityHelper->SetCityWeatherInfo(strWeatherInfo);
	}
}

void CWeatherHelper::OnGetCityPM25Info(LPVOID lpData)
{
	LPWSTR lpwText = NULL;
	DWORD dwSize = 0;
	if (QXUtf8ToUnicode((LPCSTR)lpData, lpwText, dwSize) == FALSE)
		lpwText = new WCHAR[dwSize];
	QXUtf8ToUnicode((LPCSTR)lpData, lpwText, dwSize);

	CString strCityPM25Info(lpwText);
	if (lpwText)
	{
		delete[] lpwText;
		lpwText = NULL;
	}

	CJsonObject JsonObject;
	JsonObject.FromString(strCityPM25Info);

	CString strErrorNo;

	if (JsonObject.IsMember(_T("success")))
		strErrorNo = JsonObject.GetValue(_T("success"));

	//接口获取数据成功返回1
	if (strErrorNo.CompareNoCase(_T("1")) != 0)
		return;

	CJsonObject JsonObjectResult;

	if (JsonObject.IsMember(_T("result")))
		JsonObject.GetValue(_T("result"), &JsonObjectResult);

	CString strCityAQI;
	if (JsonObjectResult.IsMember(_T("aqi")))
		strCityAQI = JsonObjectResult.GetValue(_T("aqi"));

	CString strCityAQILevel;
	if (JsonObjectResult.IsMember(_T("aqi_levnm")))
		strCityAQILevel = JsonObjectResult.GetValue(_T("aqi_levnm"));

	CString strCityAQIDesc;
	if (JsonObjectResult.IsMember(_T("aqi_remark")))
		strCityAQIDesc = JsonObjectResult.GetValue(_T("aqi_remark"));

	if (m_pCityHelper != NULL)
	{
		//格式化显示城市空气质量信息
		TCHAR szCityPM25Info[MAX_PATH];
		_stprintf_s(szCityPM25Info, _countof(szCityPM25Info), _T("%s(%s)"), strCityAQI, strCityAQILevel);
		m_pCityHelper->SetCityPM25Info(szCityPM25Info);

		//设置城市空气质量对应户外描述
		m_pCityHelper->SetCityPM25Remark(strCityAQIDesc);
	}
}