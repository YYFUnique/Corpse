#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

/************************************************************************/
/* �������ƣ�EnableDebugPriv                                                                  */
/* �������ã���������Ȩ��				     													   */
/* ���������lpszPrivilegeName Ȩ��������   ��:SE_DEBUG_NAME		   */
/*	�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE								               */
/************************************************************************/
DLL_API BOOL EnableDebugPriv(LPCTSTR lpszPrivilegeName);

/************************************************************************/
/* �������ƣ�InjectProcess                                                                         */
/* �������ã����ļ�����dllע��Զ�̽���													   */
/* ���������lpszFilePath �ļ�·��															   */
/*					 dwRemoteProcessId ����ID	         									   */
/*					dwWaitTime �ȴ�ʱ�䣬-1Ϊһֱ�ȴ���ֱ����Ӧ����λ����  */
/*	�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE								              */
/************************************************************************/
DLL_API BOOL InjectProcess(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId, DWORD dwWaitTime);