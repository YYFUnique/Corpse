#pragma once

class CIniFile
{
public:
	CIniFile(void);
	~CIniFile(void);
	BOOL GetData(LPCTSTR strSection , LPCTSTR strKey , int iSizeBuff ,TCHAR* pszBuffer);
	BOOL GetIniFileName(TCHAR* pszFilePath);
};
