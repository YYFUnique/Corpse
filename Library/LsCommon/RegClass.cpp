#include "StdAfx.h"
#include "RegClass.h"
#include "ErrorInfo.h"

CRegClass::CRegClass(HKEY hKey /* = HKEY_LOCAL_MACHINE */)
:m_hKey(NULL)
,m_hKeyRoot(hKey)
{

}

CRegClass::~CRegClass()
{
	Close();
}

void CRegClass::Close()
{
	if (m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}	
}

void CRegClass::SetHKEY(HKEY hKeyRoot )
{
	m_hKeyRoot = hKeyRoot;
}

BOOL CRegClass::OpenKey(LPCTSTR lpSubKey)
{
	ASSERT(lpSubKey);
    Close();
	DWORD dwRet = RegOpenKeyEx(m_hKeyRoot,lpSubKey,NULL, KEY_READ|KEY_WRITE,&m_hKey);
    if (dwRet != ERROR_SUCCESS)
    {
		SetErrorInfo(SYSTEM_ERROR,dwRet,_T("打开注册表项%s失败"),lpSubKey);
		return FALSE;
	}
	return TRUE;
}

BOOL CRegClass::IsRegOpen()
{
	return m_hKey != NULL ? TRUE : FALSE;
}

BOOL CRegClass::CreateKey( LPCTSTR lpSubKey )
{
	Close();
	DWORD dwRet = RegCreateKeyEx(m_hKeyRoot,lpSubKey,0L,NULL,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&m_hKey,NULL);
    
	if( ERROR_SUCCESS == dwRet )
		return TRUE;
	return FALSE;	
}

BOOL CRegClass::DeleteKey(LPCTSTR lpSubKey)
{
	ASSERT(lpSubKey);
	ASSERT(m_hKey);

	DWORD dwRet = RegDeleteKey(m_hKey,lpSubKey);
	
	if (ERROR_SUCCESS == dwRet)
		return TRUE;
	return FALSE;	
}

BOOL CRegClass::DeleteValue(LPCTSTR lpValueName)
{
	ASSERT(m_hKey);
	ASSERT(lpValueName);
	
	DWORD dwRet = RegDeleteValue(m_hKey,lpValueName);
	
	if (ERROR_SUCCESS == dwRet)
		return TRUE;

	return FALSE;	
}

BOOL CRegClass::SaveKey(LPCTSTR lpFileName)
{
	ASSERT(m_hKey);
	ASSERT(lpFileName);
	
	DWORD dwRet = RegSaveKey(m_hKey,lpFileName,NULL);
	
	if( ERROR_SUCCESS == dwRet)
		return TRUE;
	return FALSE;
}

/*============================================================
* 函 数 名：RestoreKey
* 参　　数：LPCTSTR [IN] : 待恢复的文件名
* 功能描述：通过文件名称，从其中导入注册表中
* 返 回 值：BOOL
* 抛出异常：
* 作　　者：RainLeaf, 2009年4月10日
*============================================================*/

BOOL CRegClass::RestoreKey(LPCTSTR lpFileName)
{
	ASSERT(m_hKey);
	ASSERT(lpFileName);
	
	DWORD dwRet = RegRestoreKey(m_hKey, lpFileName, 0);
	
	if (ERROR_SUCCESS == dwRet)
		return TRUE;
	return FALSE;
}

BOOL CRegClass::ReadKey(LPCTSTR lpKeyName, CString& strKeyValue)
{
	ASSERT(m_hKey);
	ASSERT(lpKeyName);
	
	DWORD dwType;
	TCHAR   szKeyValue[10*1024];
	DWORD dwSize=_countof(szKeyValue);
	DWORD dwRet = RegQueryValueEx( m_hKey, lpKeyName, NULL, &dwType, (BYTE *)szKeyValue, &dwSize );

	if( ERROR_SUCCESS == dwRet )
	{
		strKeyValue = szKeyValue;
		return TRUE;
	}
	else if (dwRet == ERROR_BUFFER_OVERFLOW)
	{
		//重新分配内存后再次读取
	}
	
	return FALSE;	
}

BOOL CRegClass::ReadKey(LPCTSTR lpKeyName, DWORD& dwVal)
{
	ASSERT(m_hKey);
	ASSERT(lpKeyName);
	
	DWORD dwType;
	DWORD dwSize=sizeof(DWORD);
	long lReturn = RegQueryValueEx(m_hKey, lpKeyName, NULL, &dwType, (BYTE*)&dwVal, &dwSize);
	
	if (ERROR_SUCCESS == lReturn)
		return TRUE;
	return FALSE;	
}

BOOL CRegClass::ReadKey(LPCTSTR lpKeyName,LPBYTE lpByte,DWORD& dwSize)
{
	DWORD dwRet = RegQueryValueEx(m_hKey,lpKeyName,NULL,NULL,lpByte,&dwSize);
	if (ERROR_SUCCESS == dwRet)
		return TRUE;
	return FALSE;
}

BOOL CRegClass::WriteKey( LPCTSTR lpKeyName, LPCTSTR lpValue )
{
	ASSERT(m_hKey);
	ASSERT(lpKeyName);
	ASSERT(lpValue );	
	DWORD dwSize = (_tcslen(lpValue)+1)*sizeof(TCHAR);
	DWORD dwRet = RegSetValueEx( m_hKey, lpKeyName, 0L, REG_SZ,(const BYTE*)lpValue, dwSize);
	
   	if( ERROR_SUCCESS == dwRet )
		return TRUE;
	return FALSE;	
}

BOOL CRegClass::WriteKey(LPCTSTR lpSubKey, DWORD dwVal)
{
	ASSERT(m_hKey);
	ASSERT(lpSubKey);
	
	DWORD dwRet = RegSetValueEx( m_hKey, lpSubKey, 0L, REG_DWORD, (const BYTE *) &dwVal, sizeof(DWORD) );
	
   	if (ERROR_SUCCESS == dwRet)
		return TRUE;
	return FALSE;	
}
