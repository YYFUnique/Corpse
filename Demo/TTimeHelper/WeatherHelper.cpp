#include "StdAfx.h"
#include "WeatherHelper.h"

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

BOOL CWeatherHelper::Init()
{
	m_pLibcurl = new CLibcurl;
	if (m_pLibcurl)
		m_pLibcurl->SetCallback(this);
}

int CTTimeHelper::Progress(DWORD dwEvent, LPVOID lpData, size_t size, size_t nmemb)
{
	if (dwEvent == HTTP_GET_CITY_LOCATION)
		OnGetCityLocation(lpData);
	else if (dwEvent == HTTP_GET_CITY_WEATHER)
		OnGetCityWeather(lpData);

	return size * nmemb;
}

void CTTimeHelper::OnGetCityLocation(LPVOID lpData)
{
	CString strWeatherInfo((LPCSTR)lpData);

	CJsonObject JsonObject;
	JsonObject.FromString(strWeatherInfo);

	int nErrorNo = -1;

	if (JsonObject.IsMember(_T("status")))
		JsonObject.GetValue(_T("status"), &nErrorNo);

	if (nErrorNo != 0)
		return 0;

	CString strAddr;
	if (JsonObject.IsMember(_T("address")))
	{
		CStdArray strAddInfo;
		strAddr = JsonObject.GetValue(_T("address"));
		AnalyseFormattedTextInfoItem(strAddr, strAddInfo, _T('|'));
		CLabelUI* pCity = (CLabelUI*)m_PaintManager.FindControl(_T("Weather"));
		if (pCity != NULL)
			pCity->SetText(strAddInfo.GetAt(2));
	}
}

void CTTimeHelper::OnGetCityWeather(LPVOID lpData)
{

}