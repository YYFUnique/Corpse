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
/* ��  ��  ����Gb32ToUtf8			                                                               */
/* �������ܣ���GB32��������ת��ΪUTF8��������									   */
/* ���������lpszGb32Text ��Ҫת����GB32�ַ�������							   */
/* ��  �� ֵ������ת��ΪUTF8�ı�������													   */
/************************************************************************/
DLL_API LPSTR Gb32ToUtf8(LPCSTR lpszGb32Text);

/************************************************************************/
/* ��  ��  ����AnalyseDbNullTerminatedText                                            */
/* �������ܣ���˫�ս��ת��Ϊ�ַ�������												   */
/* ���������lpszFormattedInfo ˫�ս���ַ���										   */
/*                  InfoArray		           ������˫�ս������								   */
/* ��  �� ֵ�������ַ�����ĸ���																   */
/************************************************************************/
DLL_API UINT AnalyseDbNullTerminatedText(LPCTSTR lpszFormattedInfo,CStdArray& InfoArray);

DLL_API void AnalyseFormattedTextInfoItem(LPCTSTR lpszFormattedInfo, CStdArray& InfoArray, TCHAR ch);
/************************************************************************/
/* ��  ��  ����QXUtf8ToUnicode						                                           */
/* �������ܣ���UTF8������ַ���ת��ΪUNICODE�����ַ���				   */
/* ���������lpszUTF8 ��Ҫת���Ķ��ֽ��ַ���										   */
/*                  lpwText	 ת������ַ���													   */
/*					 dwSize	 ����Ϊ�ַ�����������С�����غ���ʵ��ʹ�õĴ�С*/
/* ��  �� ֵ��ת���ɹ�����TRUE��ת��ʧ�ܷ���FALSE	    						   */
/************************************************************************/
DLL_API BOOL QXUtf8ToUnicode(LPCSTR lpszUTF8, LPWSTR lpwText, DWORD& dwSize);

/************************************************************************/
/* ��  ��  ����QXUnicodeToUtf8						                                           */
/* �������ܣ���UNICODE������ַ���ת��ΪUTF8�����ַ���				   */
/* ���������lpwText ��Ҫת����UNICODE�ַ���									   */
/*                  lpwText ת������ַ���														   */
/*					 dwSize  ����Ϊ�ַ�����������С�����غ���ʵ��ʹ�õĴ�С  */
/* ��  �� ֵ��ת���ɹ�����TRUE��ת��ʧ�ܷ���FALSE	    						   */
/************************************************************************/
DLL_API BOOL QXUnicodeToUtf8(LPCWSTR lpwText, LPSTR lpUTF8, DWORD& dwSize);