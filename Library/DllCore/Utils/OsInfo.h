#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_CORE_API 
#elif defined  DLL_EXPORTS
#define DLL_CORE_API __declspec(dllexport)
#else
#define DLL_CORE_API __declspec(dllimport)
#endif

/************************************************************************/
/* ��  ��  ����OsIsWow64Process                                                              */
/* �������ܣ��жϵ�ǰ�����Ƿ�������WOW64��										   */
/* �����������																						   */
/* ��  �� ֵ�������ǰ����������WOW64�з���TRUE�����򷵻�FALSE    */
/************************************************************************/

DLL_CORE_API BOOL OsIsWow64Process();

/************************************************************************/
/* ��  ��  ����OsIsVistaOrLater	                                                               */
/* �������ܣ��жϵ�ǰϵͳ�Ƿ���Vista���Ժ�ϵͳ									   */
/* �����������																						   */
/* ��  �� ֵ�������Vista���Ժ�ϵͳ����TRUE�����򷵻�FALSE				   */
/************************************************************************/
DLL_CORE_API BOOL OsIsVistaOrLater();