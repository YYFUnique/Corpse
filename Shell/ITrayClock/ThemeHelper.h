#pragma once

#include "uxtheme.h"

class CThemeHelper
{
public:
	CThemeHelper();
	virtual ~CThemeHelper();

	BOOL OpenThemeData(HWND hWnd, LPCWSTR lpszClassList);
	void CloseThemeData();
	HRESULT DrawThemeBackground(HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect);
	HRESULT DrawThemeText(HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT* pRect);
	BOOL IsThemeActive();
	BOOL IsAppThemed();

	LRESULT DrawThemeParentBackground(HWND hwnd, HDC hdc, const RECT *prc);
	BOOL IsThemeBackgroundPartiallyTransparent(int iPartId, int iStateId);
private:
	LPVOID GetProc(LPCSTR szProc, LPVOID pfnFail);

	/*typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
	static HTHEME __stdcall OpenThemeDataFail(HWND, LPCWSTR)	{return NULL;}

	typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
	static HRESULT __stdcall CloseThemeDataFail(HTHEME)	{return E_FAIL;}

	typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pRect,  const RECT* pClipRect);
	static HRESULT __stdcall DrawThemeBackgroundFail(HTHEME, HDC, int, int, const RECT*, const RECT*)	{return E_FAIL;}

	typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT* pRect);
	static HRESULT __stdcall DrawThemeTextFail(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const RECT*)	{return E_FAIL;}

	typedef BOOL(__stdcall *PFNISAPPTHEMED)();
	static BOOL __stdcall IsAppThemedFail()	{return FALSE;}

	typedef BOOL (__stdcall *PFNISTHEMEACTIVE)();
	static BOOL __stdcall IsThemeActiveFail()	{return FALSE;}*/

	HTHEME			m_hTheme;
	//HMODULE		m_hModule;
};