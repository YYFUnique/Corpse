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
//�� �� ��: UTF8ToUniCode
//��     ��: ��UTF8�����ַ���ת��ΪUNICODE�ַ���
//�������: [in] lpszUTF8��ת����LPCSTR����UTF8�ַ�����
//�� ��  ֵ: ����ת�����UNICODE�ַ��������ʹ��ANSI������ܴ�������
//====================================
LS_COMMON_API CString UTF8ToUniCode(LPCSTR lpszUTF8);

//====================================
//�� �� ��: UnicodeToUTF8
//��     ��: ��UNICODE�����ַ���ת��ΪUTF8�ַ���
//�������: [in] lpszUnicode��ת����LPCWSTR����UNICODE�ַ�����
//�� ��  ֵ: ����ת�����UTF8�ַ���
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
/* �������ƣ�RedirectionOutput                                                                */
/* �������ܣ���lpszParamΪ����ִ��lpszExeFilePath									*/
/* ����˵����lpszExeFilePath ִ���ļ�·��												   */
/*					 lpszParam	�����в���														   */
/*					 dwWaitTime  ִ�г���ȴ�ʱ�䣬-1:һֱ�ȴ�,0:���ȴ���λ����  */
/*					 strOutput			�����г������������									   */
/* ����ֵ��ִ�гɹ�����TRUE��ʧ�ܷ���FALSE*/
/************************************************************************/
LS_COMMON_API BOOL RedirectionOutput(LPCTSTR lpszExeFilePath,LPCTSTR lpszParam,DWORD dwWaitTime,CString& strOutput);

LS_COMMON_API BOOL GetUserPicturePath(LPCTSTR lpszUserName,CString& strPicturePath);

LS_COMMON_API void AnalyseFormattedTextInfoItem(LPCTSTR lpszFormattedInfo,CStringArray& InfoArray,TCHAR ch);