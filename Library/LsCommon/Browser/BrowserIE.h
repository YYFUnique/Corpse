#pragma once

#include "Browser.h"

class CBrowserIE : public CBrowser
{
public:
	CBrowserIE();
	~CBrowserIE();

public:
	virtual BOOL OpenBrowser();
	virtual BOOL StopBrowser();
	virtual BOOL SetBrowserHomePage(LPCTSTR lpszHomePage);
	virtual BOOL GetBrowserHistory(CItemInfoList& ItemInfoList);
	virtual BOOL GetBrowserCookies(CItemCookiesList& ItemCookiesList);
};