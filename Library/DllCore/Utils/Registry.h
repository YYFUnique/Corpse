#pragma once
#include <atlstr.h>

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include "StdPtr.h"

DLL_API BOOL LsRegSetValue(HKEY hKey , LPCTSTR lpszValueName , DWORD dwValue);

DLL_API BOOL LsRegSetValue(HKEY hRootKey , LPCTSTR lpszSubKey , LPCTSTR lpszValueName , DWORD dwValue);

DLL_API BOOL LsRegSetValue(HKEY hRootKey , LPCTSTR lpszSubKey , LPCTSTR lpszValueName , LPCTSTR lpszValue);

DLL_API BOOL LsRegSetValue(HKEY hKey , LPCTSTR lpszValueName , LPCTSTR lpszValue);

DLL_API BOOL LsRegSetValue(HKEY hKey , LPCTSTR lpszValueName , const CStdArray& strValueArray);

DLL_API BOOL LsRegSetValue(HKEY hKey , LPCTSTR lpszValueName , const BYTE* lpValue , DWORD dwValueLen);

DLL_API BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , DWORD& dwValue);

DLL_API BOOL LsRegQueryValue(HKEY hRootKey , LPCTSTR pszSubKey , LPCTSTR lpszValueName , DWORD& dwValue);

DLL_API BOOL LsRegQueryValue(HKEY hRootKey , LPCTSTR pszSubKey , LPCTSTR lpszValueName , CString& strValue);

DLL_API BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , CString& strValue);

DLL_API BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , CStdArray& strValueArray);

DLL_API BOOL LsRegQueryValue(HKEY hKey , LPCTSTR lpszValueName , BYTE* lpValue , DWORD& dwValueLen);
