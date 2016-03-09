#include "StdAfx.h"
#include "IniFile.h"

CIniFile::CIniFile(void)
{
	TCHAR szModuleFileName[MAX_PATH];
	GetIniFileName(szModuleFileName);
	if(PathFileExists(szModuleFileName) == FALSE)
	{
		//��������������ļ����򴴽���
		TCHAR szShowWindow[MAX_PATH]=_T("SystemSet");
		TCHAR szKeyShowWindow[MAX_PATH]=_T("ShowWindowOnStartUp");
		TCHAR szShowWindowDefault[MAX_PATH]=_T("Hide");
		WritePrivateProfileString(szShowWindow,szKeyShowWindow , szShowWindowDefault , szModuleFileName);
	}
}

BOOL CIniFile::GetData(LPCTSTR strSection , LPCTSTR strKey , int iSizeBuff ,TCHAR* pszBuffer)
{
	TCHAR szModuleFileName[MAX_PATH];
	GetIniFileName(szModuleFileName);
	GetPrivateProfileString(strSection , strKey , NULL , pszBuffer , iSizeBuff , szModuleFileName);

	if(pszBuffer == NULL)
	{
		MessageBox(NULL,_T("�������ļ�ʧ��"),_T("������"),MB_OK|MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

BOOL CIniFile::GetIniFileName(TCHAR* pszFilePath)
{
	GetModuleFileName(NULL , pszFilePath , _tcslen(pszFilePath));

	PathRenameExtension(pszFilePath,_T(".ini"));
	return TRUE;
}

CIniFile::~CIniFile(void)
{
}
