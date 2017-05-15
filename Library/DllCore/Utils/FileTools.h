#pragma once

#include <atlstr.h>

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

/************************************************************************/
/* ��  ��  ����CheckFileIsX64                                                                     */
/* �������ܣ����ָ��Ӧ�ó����Ƿ���64λӦ�ó���									   */
/* ���������lpszFilePath ��Ҫ����ļ��ľ���·��									   */
/* ��  �� ֵ�������64λӦ�÷���TRUE�����򷵻�FALSE							   */
/************************************************************************/
DLL_API BOOL CheckFileIsX64(LPCTSTR lpszFilePath);

/************************************************************************/
/* ��  ��  ����GetFileVersion                                                                     */
/* �������ܣ���ȡָ���ļ����ļ��汾��													   */
/* ���������lpszFilePath �����ļ�����·��											   */
/* ��  �� ֵ������ļ����ڣ��򷵻��ļ��汾�ţ���������ڣ�����v1.0      */
/************************************************************************/
DLL_API DWORD GetFileVersion(LPCTSTR lpszFilePath);

/************************************************************************/
/* ��  ��  ����GetVersionText                                                                    */
/* �������ܣ������ΰ汾������ת��Ϊ�ַ�������										   */
/* ���������dwVersion ��Ҫת�������ΰ汾��										   */
/* ��  �� ֵ������ת������ַ����汾��													   */
/************************************************************************/
DLL_API CString GetVersionText(DWORD dwVersion);

/************************************************************************/
/* ��  ��  ����GetModuleFileVersion                                                         */
/* �������ܣ���ȡ��ǰ�����ļ����ļ��汾												   */
/* �����������																						   */
/* ��  �� ֵ�����ص�ǰ�����ļ��ĸ�ʽ������ַ���									   */
/************************************************************************/
DLL_API CString GetModuleFileVersion();

/************************************************************************/
/* ��  ��  ����GetCommonAppDataFilePath                                              */
/* �������ܣ����������ָ��Ŀ¼����ı���·��										   */
/* ���������lpszFileDirectory �ļ��ı�������   									   */
/*					 lpszFileTitle	�ļ�Ŀ¼���ƣ�������ڶ������ͣ���������   */
/*					 lpszFilePath    ���ص��ļ�·��											   */
/* ��  �� ֵ�������ȡ�ɹ�����TRUE�����򷵻�FALSE								   */
/************************************************************************/
DLL_API BOOL GetCommonAppDataFilePath(LPCTSTR lpszFileDirectory,LPCTSTR lpszFileTitle,LPTSTR lpszFilePath);

/************************************************************************/
/* ��  ��  ����GetProgramDataFilePath                                                      */
/* �������ܣ���ȡ������ProgramDataĿ¼�µ�·��									   */
/* ���������lpszDirName Ŀ¼����														   */
/*					 lpszExtendName  �ļ���չ��(����Ҫ��".")							   */
/*					 lpszVersion		�ļ��汾��													   */
/* ��  �� ֵ�����ػ�����ļ�����															   */
/************************************************************************/
//�����ַ��������磺 C:\ProgramData\Corpse\MiniDump\PCHunter(2017-03-10)-V1.0.0.0.dmp
DLL_API CString GetProgramDataFilePath(LPCTSTR lpszDirName,LPCTSTR lpszExtendName,LPCTSTR lpszVersion /* = NULL*/);

/************************************************************************/
/* ��  ��  ����GetFilePathByLink								                                    */
/* �������ܣ���ȡ������ProgramDataĿ¼�µ�·��									   */
/* ���������lpszLinkName ����ļ�·��												   */
/*					 strFilePath  ��ݷ�ʽָ����ļ�·��									   */
/* ��  �� ֵ�������ȡ�ɹ�����TRUE�����򷵻�FALSE								   */
/************************************************************************/
DLL_API BOOL GetFilePathByLink(LPCTSTR lpszLinkName, CString& strFilePath);

/************************************************************************/
/* ��  ��  ����CreateShortcurLnkFile	     				                                   */
/* �������ܣ���ָ��·���´����ļ���ݷ�ʽ											   */
/* ���������lpszTargetFile ��ݷ�ʽָ����ļ�										   */
/*					 lpszLnkFile  ��ݷ�ʽ�ļ�����·��										   */
/* ��  �� ֵ�������ȡ�ɹ�����TRUE�����򷵻�FALSE								   */
/************************************************************************/
DLL_API BOOL CreateShortcurLnkFile(LPCTSTR lpszTargetFile, LPCTSTR lpszLnkFile);

/************************************************************************/
/* ��  ��  ����DeleteFileByTime			     				                                   */
/* �������ܣ���ָ��Ŀ¼�������ļ�������ɾ������ʱ����ļ�					   */
/* ���������lpszFolderPath ��Ҫ̽�����ļ�·��									   */
/*					 lpszFileFilter  ��Ҫָ�����˵�ͨ���									   */
/*					 dwKeepDays ��Ҫ�����ļ���ʱ�䣬����3��							*/
/* ��  �� ֵ��ִ�гɹ�����TRUE�����򷵻�FALSE									   */
/************************************************************************/
DLL_API BOOL DeleteFileByTime(LPCTSTR lpszFolderPath, LPCTSTR lpszFileFilter /*= NULL*/,DWORD dwKeepDays /*= 3*/);