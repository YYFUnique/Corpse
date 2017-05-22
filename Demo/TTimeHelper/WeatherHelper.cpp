#include "StdAfx.h"
#include "WeatherHelper.h"
#include "DllCore/Utils/StdPtr.h"
#include "DllCore/Utils/TextTools.h"

CWeatherHelper::CWeatherHelper()
{
	m_pLibcurl = NULL;
}

CWeatherHelper::~CWeatherHelper()
{
	if (m_pLibcurl != NULL)
	{
		delete m_pLibcurl;
		m_pLibcurl = NULL;
	}
}

void CWeatherHelper::Init(CCityHelper* pCityHelper)
{
	m_pLibcurl = new CLibcurl;
	if (m_pLibcurl)
		m_pLibcurl->SetCallback(this);

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
	if (m_pLibcurl == NULL)
		return FALSE;

	return m_pLibcurl->doHttpGet(HTTP_GET_CITY_LOCATION, strCityLocation);
}

BOOL CWeatherHelper::GetCityWeather(LPCTSTR lpszCityInfo)
{
	if (lpszCityInfo == NULL || _tcslen(lpszCityInfo) == 0)
		return FALSE;
	//http://api.k780.com/?app=weather.today&weaid=1&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json
	LPCTSTR lpszAppInfo = _T("weather.today");
	LPCTSTR lpszAppKey = _T("25122");
	LPCTSTR lpszSign = _T("94eaf1a9e74eddd9357947078458fa11");
	CString strCityTemperature;
	//http://api.k780.com/?app=weather.today&weaid=1&appkey=25122&sign=94eaf1a9e74eddd9357947078458fa11&format=json
	strCityTemperature.Format(_T("http://api.k780.com/?app=%s&weaid=%s&appkey=%s&sign=%s&format=json"),
		lpszAppInfo, lpszCityInfo, lpszAppKey, lpszSign);

	if (m_pLibcurl == NULL)
		return FALSE;

	CString strEncodeURL;

	URLEncode(strCityTemperature, strEncodeURL);

	return m_pLibcurl->doHttpGet(HTTP_GET_CITY_WEATHER, strEncodeURL);
}

int CWeatherHelper::Progress(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb)
{
	if (dwEvent == HTTP_GET_CITY_LOCATION)
		OnGetCityLocation(lpData);
	else if (dwEvent == HTTP_GET_CITY_WEATHER)
		OnGetCityWeather(lpData);

	return size * nmemb;
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

	CString strWeatherInfo(lpwText);
	if (lpwText)
	{
		delete[] lpwText;
		lpwText = NULL;
	}
	
	CJsonObject JsonObject;
	JsonObject.FromString(strWeatherInfo);

	CString strErrorNo;

	if (JsonObject.IsMember(_T("success")))
		strErrorNo = JsonObject.GetValue(_T("success"));

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

	if (m_pCityHelper != NULL)
	{
		m_pCityHelper->SetCityTemperature(strTemperature);
		m_pCityHelper->SetCityTemperatureRange(strTemperatureRange);
	}
}