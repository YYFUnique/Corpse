#pragma once

#include <vector>
#include <string>
#include <time.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

CHAR * UnicodeToUtf8(const WCHAR * lpszStr);
WCHAR * AnsiToUnicode(const CHAR * lpszStr);
WCHAR * Utf8ToUnicode(const CHAR * lpszStr);
CHAR * UnicodeToAnsi(const WCHAR * lpszStr);

HGLOBAL GlobalAllocByString(const CHAR * lpszText, long nLen = -1);
HGLOBAL GlobalAllocByString(const WCHAR * lpszText, long nLen = -1);

BOOL EnumSysFont(std::vector<tstring> * arrSysFont);	// 枚举系统字体
BOOL FlashWindowEx(HWND hWnd, int nCount);				// 闪烁窗口标题栏
BOOL GetTrayWndRect(RECT * lpRect);						// 获取系统任务栏区域
void FormatTime(time_t lTime, LPCTSTR lpFmt, TCHAR* lpBuf, int nLen);

BOOL IsDigit(const WCHAR * lpStr);
void Replace(CDuiString& strText, const WCHAR * lpOldStr, const WCHAR * lpNewStr);
WCHAR ToLower(WCHAR c);
void ToLower(WCHAR * lpText);
void ToLower(std::wstring& strText);
WCHAR ToUpper(WCHAR c);
void ToUpper(WCHAR * lpText);
void ToUpper(std::wstring& strText);
void EncodeHtmlSpecialChars(CDuiString& strText);		// 编码Html特殊字符
void DecodeHtmlSpecialChars(CDuiString& strText);		// 解码Html特殊字符

tstring GetBetweenString(const TCHAR * pStr, TCHAR cStart, TCHAR cEnd);
int GetBetweenInt(const TCHAR * pStr, TCHAR cStart, TCHAR cEnd, int nDefValue = 0);

tstring GetBetweenString(const WCHAR * pStr, const WCHAR * pStart, const WCHAR * pEnd);
int GetBetweenInt(const WCHAR * pStr, const WCHAR * pStart, 
				  const WCHAR * pEnd, int nDefValue = 0);

BOOL DllRegisterServer(LPCTSTR lpszFileName);
BOOL DllUnregisterServer(LPCTSTR lpszFileName);

GUID GetFileTypeGuidByExtension(const WCHAR * lpExtension);
CLSID GetEncoderClsidByExtension(const WCHAR * lpExtension);
CLSID GetEncoderClsidByFileType(REFGUID guidFileType);
CLSID GetEncoderClsidByMimeType(const WCHAR * lpMineType);
