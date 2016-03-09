#include "StdAfx.h"
#include "Browser.h"

#include "BrowserIE.h"


CBrowser::CBrowser()
{
	m_strBrowserName = _T("");
	m_BrowserType = BROWSERTYPE_UNDEFINE;
}

CBrowser::~CBrowser()
{
	//Release();
}

void CBrowser::Release()
{
	delete this;
}

CBrowser* CBrowser::CreateBrowser(BROWSERTYPE BrowserType)
{
	CBrowser* pBrowser = NULL;
	
	switch(BrowserType)
	{
		case BROWSRETYPE_IE:
				pBrowser = new CBrowserIE;
			break;
	}

	if (pBrowser != NULL)
		pBrowser->m_BrowserType = BrowserType;

	return pBrowser;
}

BOOL CBrowser::GetBrowserType(BROWSERTYPE& BrowserType)
{
	BrowserType = m_BrowserType;
	return TRUE;
}

BOOL CBrowser::OpenBrowser()
{
	return FALSE;
}

BOOL CBrowser::StopBrowser()
{
	return FALSE;
}

BOOL CBrowser::SetBrowserHomePage(LPCTSTR lpszHomePage)
{
	return FALSE;
}

BOOL CBrowser::GetBrowserHistory(CItemInfoList& ItemInfoList)
{
	return FALSE;
}

BOOL CBrowser::GetBrowserCookies(CItemCookiesList& ItemCookiesList)
{
	return FALSE;
}