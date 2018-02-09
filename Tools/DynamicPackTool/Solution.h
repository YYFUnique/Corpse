#pragma once

#include "helper/TreeView.h"
#include <atlstr.h>
#include "DllCore/Utils/DuiList.h"

typedef struct tagPROJECT_ITEM_INFO
{
	CString		strProjectName;		// 项目名称
	CString		strProjectFilePath;	// 项目文件路径
	CString		strProjectGUID;		//	项目GUID
	BOOL		bActive;					// 是否处于激活状态
}PROJECT_ITEM_INFO,*PPROJECT_ITEM_INFO;

typedef CDuiList<PROJECT_ITEM_INFO, const PROJECT_ITEM_INFO&> CProjectItemInfoList;

typedef enum tagCONTENT_TYPE
{
	CONTENT_TYPE_NONE		= 0,		// 内容未定义
	CONTENT_TYPE_PROJECT	= 1,		//	内容是项目信息
	CONTENT_TYPE_GLOBAL	= 2,		// 内容是全局信息
}CONTENT_TYPE;

class CSolution
{
public:
	CSolution();
	~CSolution();

public:
	BOOL LoadSolution(CPaintManagerUI* pManagerUI, LPCTSTR lpszSolutionFile);

	CString	 GetSolutionFilePath();
	CString GetSolutionFileVersion();
	CString GetSolutionToolVersion();
	
	void SetProjectActiveStart();
	BOOL GetSolutionProject(CProjectItemInfoList& ProjectInfo);
protected:
	BOOL _Parse(CProjectItemInfoList& ProjectInfo);
	BOOL LoadFromMem(BYTE* pByte, DWORD dwSize);

	void SetProjectInfo(LPCTSTR lpszProjectItem);
	void SetGlobalInfo(LPCTSTR lpszProjectItem);
	void SetProjectActive(LPCTSTR lpszProjectGUID, BOOL bActive);
	void SetSolutionFileVersion(LPCTSTR lpszProjectItemString);
	void SetSolutionToolVersion(LPCTSTR lpszProjectItemString);
protected:
	CString						m_strSolutionFile;						//解决方案文件路径
	CString						m_strSolutionFileVersion;
	CString						m_strSolutionToolVersion;
	CString						m_strActiveProject;
	CProjectTreeView*	m_pSolutionTreeView;
	CONTENT_TYPE		m_ContentType;
	//CPaintManagerUI*		m_pManagerUI;
	CProjectItemInfoList	m_ProjectInfo;
};