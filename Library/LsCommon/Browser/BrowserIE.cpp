#include "StdAfx.h"
#include "BrowserIE.h"
#include <Wininet.h>
#include <urlhist.h>

CBrowserIE::CBrowserIE()
{
	m_strBrowserName = _T("Internet Explorer");
}

CBrowserIE::~CBrowserIE()
{

}

BOOL CBrowserIE::OpenBrowser()
{
	return TRUE;
}

BOOL CBrowserIE::StopBrowser()
{
	return TRUE;
}

BOOL CBrowserIE::SetBrowserHomePage(LPCTSTR lpszHomePage)
{

	return TRUE;
}

BOOL CBrowserIE::GetBrowserHistory(CItemInfoList& ItemInfoList)
{
	BOOL bSucess=FALSE;

	IUrlHistoryStg2* pUrlHistoryStg2 = NULL;
	HRESULT hr	 = CoCreateInstance(CLSID_CUrlHistory, NULL, CLSCTX_INPROC_SERVER, IID_IUrlHistoryStg2, (void**)&pUrlHistoryStg2);
	if (SUCCEEDED(hr))
	{
		IEnumSTATURL* pEnumUrl=NULL;
		STATURL Status;
		Status.cbSize=sizeof(STATURL);
		ULONG celtFetched=1;
		pUrlHistoryStg2->EnumUrls(&pEnumUrl);
		pEnumUrl->Reset();
		while (pEnumUrl->Next(1,&Status,&celtFetched)==S_OK)
		{			
			ITEMINFO ItemInfo;
			ItemInfo.strTitle = Status.pwcsTitle;
			ItemInfo.strAddr = Status.pwcsUrl;
			CTime TimeLastVisitedTime(Status.ftLastVisited);
			ItemInfo.dwVisitedTime = (DWORD)TimeLastVisitedTime.GetTime();
			ItemInfoList.AddTail(ItemInfo);
		}

		pEnumUrl->Release();
		pUrlHistoryStg2->Release();
		bSucess = TRUE;
	}
	

	return bSucess;
}

BOOL CBrowserIE::GetBrowserCookies(CItemCookiesList& ItemCookiesList)
{
	BOOL bSuccess = FALSE;


	return bSuccess;
}