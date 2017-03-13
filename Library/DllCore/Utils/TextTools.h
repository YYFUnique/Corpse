#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include "StdPtr.h"

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