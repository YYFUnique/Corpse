#include "StdAfx.h"
#include "ThemeHelper.h"

#pragma comment(lib,"uxtheme.lib")

CThemeHelper::CThemeHelper()
{
	m_hTheme = NULL;
}

CThemeHelper::~CThemeHelper()
{
	if (m_hTheme != NULL)
		CloseThemeData();
}

LPVOID CThemeHelper::GetProc(LPCSTR szProc, LPVOID pfnFail)
{
	return NULL;
} 

BOOL CThemeHelper::OpenThemeData(HWND hWnd, LPCWSTR lpszClassList)
{
	m_hTheme = ::OpenThemeData(hWnd, lpszClassList);
	
	return m_hTheme != NULL;
} 

void CThemeHelper::CloseThemeData()
{	
	::CloseThemeData(m_hTheme);
	m_hTheme = NULL;
}

HRESULT CThemeHelper::DrawThemeBackground(HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect)
{
	return ::DrawThemeBackground(m_hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
} 

HRESULT CThemeHelper::DrawThemeText(HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect)
{
	return ::DrawThemeText(m_hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect);
} 

BOOL CThemeHelper::IsAppThemed()
{
	return ::IsAppThemed();
} 

BOOL CThemeHelper::IsThemeActive()
{
	return ::IsThemeActive();
}

LRESULT CThemeHelper::DrawThemeParentBackground(HWND hwnd, HDC hdc, const RECT *prc)
{
	return ::DrawThemeParentBackground(hwnd, hdc, prc);
}

BOOL CThemeHelper::IsThemeBackgroundPartiallyTransparent(int iPartId, int iStateId)
{
	return ::IsThemeBackgroundPartiallyTransparent(m_hTheme, iPartId, iStateId);
}