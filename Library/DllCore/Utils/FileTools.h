#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif


/************************************************************************/
/* ��  ��  ����SHDeleteDirectory                                                               */
/* �������ܣ�ɾ��Ŀ¼��������Ŀ¼															   */
/* ���������lpszPathName ��Ҫɾ����Ŀ¼											   */
/* ��  �� ֵ��ɾ���ɹ�����TRUE��ʧ�ܷ���FALSE									   */
/************************************************************************/
DLL_API BOOL SHDeleteDirectory(LPCTSTR lpszPathName);