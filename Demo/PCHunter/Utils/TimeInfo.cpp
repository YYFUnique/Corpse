#include "stdafx.h"
#include "TimeInfo.h"

BOOL GetBuildSystemTime(SYSTEMTIME* lpBuildTime)
{
	TCHAR szMonth[20]; 
	CString strBuildDate(__DATE__);
	CString strBuildTime(__TIME__);
	_stscanf_s((LPTSTR)(LPCTSTR)strBuildDate,  _T("%[^ ] %d %d"), szMonth, _countof(szMonth), &lpBuildTime->wDay, &lpBuildTime->wYear);
	_stscanf_s((LPTSTR)(LPCTSTR)strBuildTime, _T("%d:%d:%d"), &lpBuildTime->wHour, &lpBuildTime->wMinute, &lpBuildTime->wSecond);

	LPCTSTR lpszMonth[] = {_T("Jan"),_T("Feb"),_T("Mar"),
		_T("Apr"),_T("May"),_T("Jun"),
		_T("Jul"),_T("Aug"),_T("Sep"),
		_T("Oct"),_T("Nov"),_T("Dec")};

	for (int n=0; n<_countof(lpszMonth); ++n)
	{
		if (_tcsicmp(lpszMonth[n],szMonth) == 0)
		{
			lpBuildTime->wMonth = n+1;
			return TRUE;
		}		
	}

	return FALSE;
}