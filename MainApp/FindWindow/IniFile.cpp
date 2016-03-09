#include "StdAfx.h"
#include "IniFile.h"

CIniFile::CIniFile(void)
{
	TCHAR szModuleFileName[MAX_PATH];
	GetIniFileName(szModuleFileName);
	if(PathFileExists(szModuleFileName) == FALSE)
	{
		//如果不存在配置文件，则创建。
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
		MessageBox(NULL,_T("打开配置文件失败"),_T("出错了"),MB_OK|MB_ICONERROR);
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
