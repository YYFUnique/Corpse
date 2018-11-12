#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include "StdPtr.h"
#include <atlstr.h>

/************************************************************************/
/* 函  数  名：Gb32ToUtf8			                                                               */
/* 函数功能：将GB32编码内容转换为UTF8编码内容									   */
/* 输入参数：lpszGb32Text 需要转换的GB32字符串内容							   */
/* 返  回 值：返回转换为UTF8的编码内容													   */
/************************************************************************/
DLL_API LPSTR Gb32ToUtf8(LPCSTR lpszGb32Text);

/************************************************************************/
/* 函  数  名：AnalyseDbNullTerminatedText                                            */
/* 函数功能：将双终结符转换为字符串数组												   */
/* 输入参数：lpszFormattedInfo 双终结符字符串										   */
/*                  InfoArray		           解析的双终结符数组								   */
/* 返  回 值：返回字符数组的个数																   */
/************************************************************************/
DLL_API UINT AnalyseDbNullTerminatedText(LPCTSTR lpszFormattedInfo,CStdArray& InfoArray);

DLL_API void AnalyseFormattedTextInfoItem(LPCTSTR lpszFormattedInfo, CStdArray& InfoArray, TCHAR ch);
/************************************************************************/
/* 函  数  名：QXUtf8ToUnicode						                                           */
/* 函数功能：将UTF8编码的字符串转换为UNICODE编码字符串				   */
/* 输入参数：lpszUTF8 需要转换的多字节字符串										   */
/*                  lpwText	 转换后的字符串													   */
/*					 dwSize	 传入为字符串缓冲区大小，返回后是实际使用的大小*/
/* 返  回 值：转换成功返回TRUE，转换失败返回FALSE	    						   */
/************************************************************************/
DLL_API BOOL QXUtf8ToUnicode(LPCSTR lpszUTF8, LPWSTR lpwText, DWORD& dwSize);

/************************************************************************/
/* 函  数  名：QXUnicodeToUtf8						                                           */
/* 函数功能：将UNICODE编码的字符串转换为UTF8编码字符串				   */
/* 输入参数：lpwText 需要转换的UNICODE字符串									   */
/*                  lpwText 转换后的字符串														   */
/*					 dwSize  传入为字符串缓冲区大小，返回后是实际使用的大小  */
/* 返  回 值：转换成功返回TRUE，转换失败返回FALSE	    						   */
/************************************************************************/
DLL_API BOOL QXUnicodeToUtf8(LPCWSTR lpwText, LPSTR lpUTF8, DWORD& dwSize);

DLL_API BOOL URLEncode(LPCTSTR lpszSrcURL, CString& strEncodeURL);

DLL_API BOOL URLDecode(LPCSTR lpszEncodeURL, CString& strDecodeURL);

DLL_API BOOL StrFormatNumber(ULONGLONG ullNumber, LPTSTR lpszOutput, UINT cchBuf);

DLL_API BOOL StrFromTimeIntervalEx(LPTSTR pszOut, UINT cchMax, DWORD dwTimeMS, int digits);