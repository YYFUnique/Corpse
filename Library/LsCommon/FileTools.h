#pragma once
#include <Shlwapi.h>
#include "DebugInfo.h"
#include "CommonFunc.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#define			FILE_VERSION_FileVersion			_T("FileVersion")
#define			FILE_VERSION_FileDescription  _T("FileDescription")
#define			FILE_VERSION_CompanyName _T("CompanyName")
#define			FILE_VERSION_ProductName		_T("ProductName")

typedef struct _LANGANDCODEPAGE
{
	WORD wLanguage;
	WORD wCodePage;
}LANGANDCODEPAGE,*PLANGANDCODEPAGE;

#include <shlwapi.h>

//==================================================================
//�� �� ��: GetCommonAppDataFilePath
//��    ��: ��ȡ\Documents and Settings\All Users\Application Data\lonsin\Languard
//			�µ��ļ�·�������û�Ȩ��Ҫ��(Ŀ¼�������򴴽�Ŀ¼)
//�������: [in] lpszFileDirectory, ����ӵ��ļ���
//			[in] lpszFileTitle, ����ӵ��ļ���
//			[in] lpszFilePath, ��ŵõ��������ļ�·���Ļ�����
//�� �� ֵ: �޷���ֵ
//==================================================================
LS_COMMON_API void GetCommonAppDataFilePath(LPCTSTR lpszFileDirectory,LPCTSTR lpszFileTitle,LPTSTR lpszFilePath);

//==================================================================
//�� �� ��: GetAppDataFileName
//��    ��: ��ȡ��־�ļ�·��,(����C:\Documents and Settings\All Users.WINDOWS\
//			Application Data\Lonsin\�����ն˰�ȫ����ϵͳ\"lpszDirName"\�ļ���."lpszExtendName"
//�������: 
//				lpszDirName			Ҫ��ȡ��lpszDirName�ļ���·��
//				lpszExtendName		��ȡlpszExtendName���͵��ļ�
//				lpszVersion				�������ϵͳ������汾��
//�� �� ֵ: ���ػ�ȡ������־�ļ���·��
//==================================================================
LS_COMMON_API CString GetFilePath(LPCTSTR lpszFileName,LPCTSTR lpszExtendName,LPCTSTR lpszInfo = NULL);

//==================================================================
//�� �� ��: GetFileVersion
//��    ��: ��ȡ�ļ��汾��Ϣ
//�������: [in] lpszFilePath, �ļ�·��,�ò���Ϊ����Ϊ��ǰģ��·��
//�� �� ֵ: ����DWORD�͵İ汾����Ϣ
//==================================================================
LS_COMMON_API DWORD GetFileVersion(LPCTSTR lpszFilePath);

LS_COMMON_API BOOL GetFileVersionEx(LPCTSTR lpszFilePath,LPCTSTR FileVersion,CString& strFileVersionInfo);

/*LS_COMMON_API CString GetFileVersionInfoText(FILE_VERSION FileVersion);*/
//==================================================================
//�� �� ��: GetVersionText
//��    ��: ��dword�͵İ汾��,ת��Ϊ�ַ�����
//�������: [in] dwVersion, �汾��
//�� �� ֵ: �����ַ����͵İ汾����Ϣ
//==================================================================
LS_COMMON_API CString GetVersionText(DWORD dwVersion);

//==================================================================
//�� �� ��: LsGetMainAppVersion
//��     ��: ��ȡ��ǰ������汾��
//�������: ��
//�� �� ֵ: ����һ���޷��ų�����ֵ
//==================================================================
LS_COMMON_API DWORD LsGetMainAppVersion();

//==================================================================
//�� �� ��: LsGetMainAppVersionText
//��     ��: ��ȡ��ǰ������汾��
//�������: ��
//�� �� ֵ: �������޷��ų����͹������������汾�ţ���(V1.0.1.42)
//==================================================================
LS_COMMON_API CString LsGetMainAppVersionText();

//==================================================================
//�� �� ��: GetFileSizeByName
//��     ��: ��ȡָ���ļ��Ĵ�С
//�������: lpszFilePath �ļ�·��
//�� �� ֵ: �����ļ���С
//==================================================================
LS_COMMON_API ULONGLONG GetFileSizeByName(LPCTSTR lpszFilePath);

//==================================================================
//�� �� ��: GetFileLastModifyTime
//��     ��: ��ȡ�ļ�������޸�ʱ��
//�������: lpszFilePath �ļ�·��
//�� �� ֵ: �����ļ��޸�ʱ��
//==================================================================
LS_COMMON_API ULONGLONG GetFileLastModifyTime(LPCTSTR lpszFilePath);

LS_COMMON_API LPSHELLFOLDER GetSpecialFolder(HWND hwnd, UINT nFolderCsidl);

LS_COMMON_API LPSHELLFOLDER GetSpecialFolder(HWND hWnd,LPCTSTR lpszSepcialFolderPath);

LS_COMMON_API void	DoDrop(LPDATAOBJECT lpDataObject, LPDROPTARGET lpDropTarget);

LS_COMMON_API LPITEMIDLIST PidlFromPath(HWND hWnd, LPCTSTR lpszPath);

LS_COMMON_API HRESULT GetUIObjectOfPath(HWND hWnd, LPCTSTR pszPath, REFIID riid, LPVOID *ppvOut);

LS_COMMON_API HRESULT GetUIObjectOfAbsPidl(HWND hWnd, LPITEMIDLIST pidl, REFIID riid, LPVOID *ppvOut);

LS_COMMON_API BOOL GetFilePathByLink(LPCTSTR lpszLinkName,CString& strFilePath);

LS_COMMON_API BOOL SaveBitmapFile(HBITMAP hBitmap,LPCTSTR lpszFilePath);