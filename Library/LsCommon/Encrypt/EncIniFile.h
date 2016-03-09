#pragma once
// EncIniFile.h : header fi
//
#include "FileDataItem.h"


#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncIniFile window

class LS_COMMON_API CEncIniFile : public CFileDataItem
{
// Construction
public:
	CEncIniFile();

// Attributes
public:

// Operations
public:

//==================================================================
//函 数 名: GetFileName
//功    能: 获取类最近一次操作的文件名
//输入参数: 无输入参数
//返 回 值: 返回类最近一成操作的文件名
//==================================================================
	CString& GetFileName();

//==================================================================
//函 数 名: SaveDataToFile
//功    能: 保存数据到指定配置文件中
//输入参数: [in] dwFileVersion,保存配置文件设置的版本号(只支持为1的版本号)
//			[in] lpszFileName, 配置文件的全路径文件名
//			[in] lpszMagicText, 魔法字
//返 回 值: 配置文件数据保存成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL SaveDataToFile(DWORD dwFileVersion,LPCTSTR lpszFileName,LPCTSTR lpszMagicText);

//==================================================================
//函 数 名: LoadDataFromFile
//功    能: 导出配置文件数据
//输入参数: [in] 配置文件的全路径文件名
//			[in] 配置文件的魔法字
//返 回 值: 导出配置文件数据成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL LoadDataFromFile(LPCTSTR lpszFileName,LPCTSTR lpszMagicText);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLsEncIniFile)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEncIniFile();

	// Generated message map functions
protected:
	DWORD m_dwFileVersion;
	CString m_strFileName;
	static const BYTE m_byEncryptKeyIv[48];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
