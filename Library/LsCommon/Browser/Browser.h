#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

typedef struct tgaITEMINFO
{	
	CString strTitle;
	CString strAddr;
	CString strExtendPath;
	DWORD dwVisitedTime;
}ITEMINFO,*PITEMINFO;

typedef CList<ITEMINFO,ITEMINFO&> CItemInfoList;

/*typedef struct tgaITEMCOOKIES
{
	CString strHost;
	CString strTitle;
}ITEMCOOKIES,*PITEMCOOKIES;*/

typedef ITEMINFO ITEMCOOKIES;

typedef CItemInfoList CItemCookiesList;

typedef enum tgaBROWSERTYPE
{
	BROWSERTYPE_UNDEFINE = 0,
	BROWSRETYPE_IE,
	BROWSERTYPE_360,
	BROWSERTYPE_FIREFOX,
	BROWSERTYPE_CHORM,
}BROWSERTYPE;

class LS_COMMON_API CBrowser
{
public:
	CBrowser();
	virtual ~CBrowser();
	void Release();
public:
	virtual BOOL OpenBrowser();
	virtual BOOL StopBrowser();
	virtual BOOL SetBrowserHomePage(LPCTSTR lpszHomePage);
	virtual BOOL GetBrowserHistory(CItemInfoList& ItemInfoList);
	virtual BOOL GetBrowserCookies(CItemCookiesList& ItemCookiesList);
	BOOL GetBrowserType(BROWSERTYPE& BrowserType);
	static CBrowser* CreateBrowser(BROWSERTYPE BrowserType);
protected:
	CString m_strBrowserName;
	BROWSERTYPE m_BrowserType;
};