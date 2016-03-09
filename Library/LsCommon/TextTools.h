#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif
#include "NtDll.h"

typedef enum tagCURRENT_FORMAT_TIME
{
	CURRENT_FORMAT_TIME_Normal = 0 ,
	CURRENT_FORMAT_TIME_LongTime,
}CURRENT_FORMAT_TIME;

//====================================
//函 数 名: UTF8ToUniCode
//功     能: 将UTF8编码字符串转换为UNICODE字符串
//输入参数: [in] lpszUTF8待转换的LPCSTR类型UTF8字符串，
//返 回  值: 返回转换后的UNICODE字符串，如果使用ANSI编码可能存在问题
//====================================
LS_COMMON_API CString UTF8ToUniCode(LPCSTR lpszUTF8);

//====================================
//函 数 名: UnicodeToUTF8
//功     能: 将UNICODE编码字符串转换为UTF8字符串
//输入参数: [in] lpszUnicode待转换的LPCWSTR类型UNICODE字符串，
//返 回  值: 返回转换后的UTF8字符串
//====================================
LS_COMMON_API CStringA UnicodeToUTF8(LPWSTR lpszUnicode);

LS_COMMON_API CStringA UnicodeToAnsi(LPWSTR lpszUnicode);

LS_COMMON_API void AnalyseDbNullTerminatedText(LPCTSTR lpszFormattedInfo,CStringArray& InfoArray);

LS_COMMON_API CString ConvertBinToString(const void* pBuffer,DWORD dwBufferLen);

LS_COMMON_API CString FormatNumSizeToStringSize(ULONGLONG ulFileSizeInByte);

LS_COMMON_API ULONGLONG ConvertStringSizeToNum(LPCTSTR lpszStringSize);

LS_COMMON_API CString FormatNumToString(DWORD dwNum);

LS_COMMON_API CString FormatNumToHexString(DWORD dwNum);

LS_COMMON_API CString FormatNumToHexStringNoPre(DWORD dwNum);

LS_COMMON_API CString FormatTimeToString(ULONGLONG ulTime);

LS_COMMON_API CString GetHandleDes(HANDLE dwObjType,OBJECT_INFORMATION_CLASS dwQueryType);

LS_COMMON_API CString GetGuidString(GUID DevGuid);

LS_COMMON_API CString GetCurrentFormatTime(CURRENT_FORMAT_TIME CurrentFormatTime);

LS_COMMON_API BOOL CopyDataToClipboard(DWORD dwDataType,HWND hWndNewOwner,LPCTSTR lpszText);

/************************************************************************/
/*                                                                      */
/************************************************************************/
LS_COMMON_API BOOL PrintNumberWithSeparator(ULONGLONG ullNumber, TCHAR szSeparator ,CString& strNumWithSeparator);


/************************************************************************/
/* 函数名称：RedirectionOutput                                                                */
/* 函数功能：以lpszParam为参数执行lpszExeFilePath									*/
/* 参数说明：lpszExeFilePath 执行文件路径												   */
/*					 lpszParam	命令行参数														   */
/*					 dwWaitTime  执行程序等待时间，-1:一直等待,0:不等待单位毫秒  */
/*					 strOutput			命令行程序输出的内容									   */
/* 返回值：执行成功返回TRUE，失败返回FALSE*/
/************************************************************************/
LS_COMMON_API BOOL RedirectionOutput(LPCTSTR lpszExeFilePath,LPCTSTR lpszParam,DWORD dwWaitTime,CString& strOutput);

LS_COMMON_API BOOL GetUserPicturePath(LPCTSTR lpszUserName,CString& strPicturePath);

LS_COMMON_API void AnalyseFormattedTextInfoItem(LPCTSTR lpszFormattedInfo,CStringArray& InfoArray,TCHAR ch);