#include "stdafx.h"
#include "Register.h"
#include "../ErrorInfo.h"

BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , CString& strValue)
{
	TCHAR szValue[4*1024] = {0};
	DWORD dwValueLen = sizeof(szValue);
	LPTSTR lpszOutputString = szValue;
	LONG nErrorCode = RegQueryValueEx(hKey,lpszValueName,NULL,NULL,(BYTE*)lpszOutputString,&dwValueLen);

	if (nErrorCode != ERROR_SUCCESS && nErrorCode != ERROR_MORE_DATA)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("��ȡע����ֵʧ��:"));
		return FALSE;
	}

	if (dwValueLen > sizeof(szValue))
	{
		lpszOutputString = new TCHAR[dwValueLen/sizeof(TCHAR)];
		if (lpszOutputString == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("�����ڴ�ʧ��:"));
			return FALSE;
		}
		nErrorCode = RegQueryValueEx(hKey,lpszValueName,NULL,NULL,(BYTE*)lpszOutputString,&dwValueLen);
		if (nErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("��ȡע����ֵʧ��:"));
			delete[] lpszOutputString;
			return FALSE;
		}
	}
	strValue = lpszOutputString;

	if (dwValueLen > sizeof(szValue))
	{
		delete[] lpszOutputString;
	}

	return TRUE;
}