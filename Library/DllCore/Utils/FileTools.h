#pragma once

#ifdef LS_STATIC_LIB_CALL
#define YCLIB_CORE_API 
#elif defined  YCLIBCORE_EXPORTS
#define YCLIB_CORE_API __declspec(dllexport)
#else
#define YCLIB_CORE_API __declspec(dllimport)
#endif


/************************************************************************/
/* ��  ��  ����SHDeleteDirectory                                                               */
/* �������ܣ�ɾ��Ŀ¼��������Ŀ¼															   */
/* ���������lpszPathName ��Ҫɾ����Ŀ¼											   */
/* ��  �� ֵ��ɾ���ɹ�����TRUE��ʧ�ܷ���FALSE									   */
/************************************************************************/
YCLIB_CORE_API BOOL SHDeleteDirectory(LPCTSTR lpszPathName);