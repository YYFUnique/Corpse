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