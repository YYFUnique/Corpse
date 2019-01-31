#include "StdAfx.h"
#include "Registry.h"
#include "ErrorInfo.h"

CString GetRegistryPath(LPCTSTR lpszAppKeyName)
{
	CString strRegistryPath;
	strRegistryPath.Format(_T("Software\\Corpse\\%s"), lpszAppKeyName);

	return strRegistryPath;
}

BOOL LsRegSetValue(HKEY hKey , LPCTSTR lpszValueName , DWORD dwValue)
{
	LONG nErrorCode = RegSetValueEx(hKey,lpszValueName,NULL,REG_DWORD,(BYTE *)&dwValue,sizeof(DWORD)) ;
	if (nErrorCode != ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("设置注册表键值失败"));
		return FALSE;
	}
	return TRUE;
}

BOOL LsRegSetValue(HKEY hRootKey , LPCTSTR lpszSubKey , LPCTSTR lpszValueName , DWORD dwValue, DWORD dwRegSam /*= KEY_SET_VALUE*/)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = 0;

	do
	{
		DWORD dwErrorCode = RegCreateKeyEx(hRootKey , lpszSubKey , 0, NULL, REG_OPTION_NON_VOLATILE, dwRegSam, NULL, &hKey, NULL);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR , dwErrorCode , _T("写注册表时打开键失败"));
			break;
		}

		if (LsRegSetValue(hKey , lpszValueName , dwValue) == FALSE)
			break;

		bSuccess = TRUE;
	} while (0);

	if (hKey)
		RegCloseKey(hKey);

	return bSuccess;
}

BOOL LsRegSetValue(HKEY hRootKey , LPCTSTR lpszSubKey , LPCTSTR lpszValueName , LPCTSTR lpszValue, DWORD dwRegSam /*= KEY_SET_VALUE*/)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = 0;

	do
	{
		DWORD dwErrorCode = RegCreateKeyEx(hRootKey , lpszSubKey , 0, NULL, REG_OPTION_NON_VOLATILE, dwRegSam, NULL, &hKey, NULL);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR , dwErrorCode , _T("写注册表时打开键失败"));
			break;
		}

		if (LsRegSetValue(hKey , lpszValueName , lpszValue) == FALSE)
			break;

		bSuccess = TRUE;
	} while (0);

	if (hKey)
		RegCloseKey(hKey);

	return bSuccess;
}

BOOL LsRegSetValue(HKEY hKey , LPCTSTR lpszValueName , LPCTSTR lpszValue)
{
	DWORD dwLen=((DWORD)(_tcslen(lpszValue)+1)*sizeof(TCHAR));
	LONG nErrorCode = RegSetValueEx(hKey,lpszValueName,NULL,REG_SZ,(BYTE*)lpszValue,dwLen);
	if (nErrorCode != ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("设置注册表键值失败"));
		return FALSE;
	}
	return TRUE;
}

BOOL LsRegSetValue(HKEY hKey , LPCTSTR lpszValueName , const CStdArray& strValueArray)
{
	BOOL bRet = TRUE;

	int nStringCount=(int)strValueArray.GetSize();
	DWORD dwBufferLen = 0;

	for (int i=0; i<nStringCount; i++)
	{
		const CString& strText=strValueArray.GetAt(i);
		DWORD dwTextLen=strText.GetLength()+1;
		dwBufferLen += dwTextLen;
	}
	dwBufferLen +=1;

	const DWORD dwLen = 4*1024;
	TCHAR szBuff[dwLen];
	LPTSTR lpszOutputString = szBuff;

	if (dwBufferLen > dwLen)
	{
		lpszOutputString = new TCHAR[dwBufferLen];
		if (lpszOutputString == NULL )
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("分配内存失败"));
			return FALSE;
		}
	}

	DWORD dwBufferOffset=0;

	for (int n=0; n<nStringCount; ++n)
	{
		const CString& strText = strValueArray.GetAt(n);
		DWORD dwTextLen=strText.GetLength()+1;
		_tcscpy_s(lpszOutputString+dwBufferOffset, strText.GetLength(), strText);
		dwBufferOffset+=dwTextLen;
	}
	lpszOutputString[dwBufferOffset] = 0;

	LONG nErrorCode = RegSetValueEx(hKey,lpszValueName,NULL,REG_MULTI_SZ,(BYTE*)lpszOutputString,dwBufferLen);
	if (nErrorCode != ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("设置注册表键值失败"));
		bRet = FALSE;
	}

	if (dwBufferLen > dwLen)
	{
		delete[] lpszOutputString;
	}

	return bRet;
}

BOOL LsRegSetValue(HKEY hKey , LPCTSTR lpszValueName , const BYTE* lpValue , DWORD dwValueLen)
{
	LONG nErrorCode = RegSetValueEx(hKey,lpszValueName,NULL,REG_BINARY, lpValue, dwValueLen) ;
	if (nErrorCode != ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("设置注册表键值失败"));
		return FALSE;
	}
	return TRUE;
}

BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , DWORD& dwValue)
{
	DWORD dwValueLen = sizeof(DWORD);
	LONG nErrorCode = RegQueryValueEx(hKey,lpszValueName,NULL,NULL,(BYTE*)&dwValue,&dwValueLen);

	if (nErrorCode != ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("获取注册表键值失败"));
		return FALSE;
	}

	return TRUE;
}

BOOL LsRegQueryValue(HKEY hRootKey , LPCTSTR pszSubKey , LPCTSTR lpszValueName , CString& strValue, DWORD dwRegSam /*= KEY_READ*/)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;
	do 
	{
		DWORD dwErrorCode = RegOpenKeyEx(hRootKey , pszSubKey , 0 , dwRegSam , &hKey);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR , dwErrorCode , _T("读取注册表时打开键失败"));
			break;
		}

		if (LsRegQueryValue(hKey , lpszValueName , strValue) == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (hKey != NULL)
		RegCloseKey(hKey);

	return bSuccess;
}

BOOL LsRegQueryValue(HKEY hRootKey , LPCTSTR pszSubKey , LPCTSTR lpszValueName , DWORD& dwValue, DWORD dwRegSam /*= KEY_READ*/)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = 0;

	do 
	{
		DWORD dwErrorCode = RegOpenKeyEx(hRootKey , pszSubKey , 0 , dwRegSam , &hKey);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR , dwErrorCode , _T("读取注册表时打开键失败"));
			break;
		}

		if (LsRegQueryValue(hKey , lpszValueName , dwValue) == FALSE)
			break;

		bSuccess = TRUE;
	} while (0);

	if (hKey)
		RegCloseKey(hKey);

	return bSuccess;
}

BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , CString& strValue)
{
	TCHAR szValue[4*1024] = {0};
	DWORD dwValueLen = sizeof(szValue);
	LPTSTR lpszOutputString = szValue;
	LONG nErrorCode = RegQueryValueEx(hKey,lpszValueName,NULL,NULL,(BYTE*)lpszOutputString,&dwValueLen);

	if (nErrorCode != ERROR_SUCCESS && nErrorCode != ERROR_MORE_DATA)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("获取注册表键值失败"));
		return FALSE;
	}

	if (dwValueLen > sizeof(szValue))
	{
		lpszOutputString = new TCHAR[dwValueLen/sizeof(TCHAR)];
		if (lpszOutputString == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("分配内存失败"));
			return FALSE;
		}
		nErrorCode = RegQueryValueEx(hKey,lpszValueName,NULL,NULL,(BYTE*)lpszOutputString,&dwValueLen);
		if (nErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("获取注册表键值失败"));
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

BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , CStdArray& strValueArray)
{
	const DWORD dwInitialLen = 4*1024;

	DWORD dwBufLen = dwInitialLen; 
	TCHAR szValue[dwInitialLen/sizeof(TCHAR)] ={0};
	LPTSTR lpszOutputString= szValue;
	LONG nErrorCode = RegQueryValueEx(hKey,lpszValueName,NULL,NULL,(BYTE*)lpszOutputString,&dwBufLen);

	if (nErrorCode != ERROR_SUCCESS && nErrorCode != ERROR_MORE_DATA)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("获取注册表键值失败"));
		return FALSE;
	}

	if (dwBufLen > dwInitialLen)
	{
		lpszOutputString = new TCHAR[dwBufLen/sizeof(TCHAR)];
		if (lpszOutputString == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("分配内存失败"));
			return FALSE;
		}

		nErrorCode = RegQueryValueEx(hKey,lpszValueName,NULL,NULL,(BYTE*)lpszOutputString,&dwBufLen);
		if (nErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("获取注册表键值失败"));
			delete[] lpszOutputString;
			return FALSE;
		}
	}

	strValueArray.RemoveAll();
	if (!(*lpszOutputString == '\0' && *(lpszOutputString+1) == '\0'))
	{
		while (1)
		{
			strValueArray.Add(lpszOutputString);
			lpszOutputString+=_tcslen(lpszOutputString)+1;
			if (*lpszOutputString == '\0')
				break;
		}
	}

	if (dwBufLen > dwInitialLen)
	{
		delete[] lpszOutputString;
	}

	return TRUE;
}

BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , BYTE* lpValue , DWORD& dwValueLen)
{
	LONG nErrorCode = RegQueryValueEx(hKey,lpszValueName,NULL,NULL,lpValue,&dwValueLen);

	if (nErrorCode != ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR,nErrorCode,_T("获取注册表键值失败"));
		return FALSE;
	}

	return TRUE;
}

BOOL RegSetVolatile(LPCTSTR lpszSubKey, LPCTSTR lpszKeyName, DWORD dwValue)
{
	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;
	do 
	{

		DWORD dwRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpszSubKey, 0, NULL, REG_OPTION_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL);
		if (dwRet == ERROR_SUCCESS)
			RegSetValueEx(hKey, lpszKeyName, 0, REG_DWORD, (PBYTE)&dwValue, sizeof(DWORD));

		bSuccess = TRUE;
	} while (FALSE);

	if (hKey != NULL)
		RegCloseKey(hKey);

	return bSuccess;
}