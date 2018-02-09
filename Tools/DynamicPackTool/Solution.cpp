#include "StdAfx.h"
#include "Solution.h"
#include "DllCore/Utils/StdPtr.h"
#include "DllCore/Utils/TextTools.h"

#define MAX_UINT32					(4*1024*1024)		//32位系统最大表示范围

// 工程文件版本内容，后面接具体版本
#define PROJECT_FILE_VERSION		_T("Microsoft DynamicPackTool Studio Project File")
// 工程文件对应开发工具
#define PROJECT_TOOL_VERSION		_T("# DynamicPackTool Studio")
//	全局配置信息，处于激活状态的项目
#define GLOBAL_PROJECT_ACTIVE	_T("GlobalSection(SolutionConfigActive)")

CSolution::CSolution()
{
	m_ContentType = CONTENT_TYPE_NONE;
	m_pSolutionTreeView = NULL;
}

CSolution::~CSolution()
{

}

BOOL CSolution::LoadSolution(CPaintManagerUI* pManagerUI, LPCTSTR lpszSolutionFile)
{
	BOOL bSuccess = FALSE;
	m_strSolutionFile = lpszSolutionFile;

	do 
	{
		if (PathFileExists(m_strSolutionFile) == FALSE)
			break;

		//解析解决方案文件，获取项目
		if (_Parse(m_ProjectInfo) == FALSE)
			break;

		m_pSolutionTreeView = new CProjectTreeView(this, pManagerUI);
		if (m_pSolutionTreeView == NULL)
			break;

		bSuccess = m_pSolutionTreeView->LoadSolution();

	}while(FALSE);

	return bSuccess;
}

BOOL CSolution::_Parse(CProjectItemInfoList& ProjectInfo)
{
	BOOL bSuccess = FALSE;
	BYTE* lpByte = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	do 
	{
		hFile = CreateFile(m_strSolutionFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			break;

		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if (dwSize == 0)
			break;

		if (dwSize > MAX_UINT32)
			break;

		DWORD dwRead = 0;
		lpByte = new BYTE[dwSize];
		if (::ReadFile(hFile, lpByte, dwSize, &dwRead, NULL) == FALSE)
			break;

		if (dwRead != dwSize)
			break;
		
		bSuccess = LoadFromMem(lpByte, dwSize);
	} while (FALSE);

	if (lpByte != NULL)
	{
		delete[] lpByte;
		lpByte = NULL;
	}

	if (hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return bSuccess;
}

BOOL CSolution::LoadFromMem(BYTE* pByte, DWORD dwSize)
{
	BOOL bSuccess = FALSE;
	do 
	{
		CString strProjectFile((LPCSTR)pByte, dwSize);
		CStdArray strProjectItem;
		AnalyseFormattedTextInfoItem(strProjectFile, strProjectItem, _T('\n'));

		m_ContentType = CONTENT_TYPE_NONE;

		for (int n=0; n<strProjectItem.GetSize(); ++n)
		{
			LPCTSTR lpszProjectItem = strProjectItem.GetAt(n);
			if (_tcsnicmp(lpszProjectItem, PROJECT_FILE_VERSION, _tcslen(PROJECT_FILE_VERSION)) == 0)
				SetSolutionFileVersion(lpszProjectItem);
			else if (_tcsnicmp(lpszProjectItem, PROJECT_TOOL_VERSION, _tcslen(PROJECT_TOOL_VERSION)) == 0)
				SetSolutionToolVersion(lpszProjectItem);
			else if (m_ContentType == CONTENT_TYPE_PROJECT || _tcsnicmp(lpszProjectItem, _T("Project"), 7) == 0)
				SetProjectInfo(lpszProjectItem);
			else if (m_ContentType == CONTENT_TYPE_GLOBAL || _tcsnicmp(lpszProjectItem, _T("Global"), 6) == 0)
				SetGlobalInfo(lpszProjectItem);
		}

		bSuccess = m_ProjectInfo.IsEmpty() == FALSE;
	} while (FALSE);

	return TRUE;
}

void CSolution::SetProjectInfo(LPCTSTR lpszSolutionItem)
{
	CString strSolutionItem(lpszSolutionItem);
	strSolutionItem.Remove(_T(' '));			//	移除空格
	strSolutionItem.Remove(_T('"'));			//	移除"
	strSolutionItem.TrimLeft(_T("\t"));			// 移除Tab缩进
	strSolutionItem.TrimRight(_T("\r\n"));	// 移除回车换行
	// 项目信息结束
	if (strSolutionItem.CompareNoCase(_T("EndProjectSection")) == 0)
		return;

	if (strSolutionItem.CompareNoCase(_T("EndProject")) == 0)
	{
		m_ContentType = CONTENT_TYPE_NONE;
		return;
	}

	////下列函数的格式化参数请查看如下文章
	////http://www.cnblogs.com/gmh915/archive/2009/09/30/1576995.html

	TCHAR szProjectInfo[MAX_PATH];
	TCHAR szProjectName[MAX_PATH];
	TCHAR szProjectPath[MAX_PATH];
	TCHAR szProjectGUID[MAX_PATH];

	_stscanf_s(strSolutionItem, _T("%[^=]= %[^,],%[^,],%[^,]"),
												szProjectInfo, _countof(szProjectInfo),
												szProjectName, _countof(szProjectName),
												szProjectPath, _countof(szProjectPath),
												szProjectGUID, _countof(szProjectGUID));

	//该项目不是指定项目
	if (_tcsicmp(szProjectInfo,_T("Project({8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942})")) != 0)
		return;

	PROJECT_ITEM_INFO ProjectItemInfo;
	ProjectItemInfo.strProjectName = szProjectName;
	ProjectItemInfo.strProjectFilePath = szProjectPath;
	ProjectItemInfo.strProjectGUID = szProjectGUID;
	ProjectItemInfo.bActive = FALSE;

	m_ProjectInfo.AddTail(ProjectItemInfo);

	m_ContentType = CONTENT_TYPE_PROJECT;
}

void CSolution::SetGlobalInfo(LPCTSTR lpszGlobalInfo) 
{
	CString strGlobalItem(lpszGlobalInfo);
	strGlobalItem.Remove(_T(' '));
	strGlobalItem.Remove(_T('"'));
	strGlobalItem.TrimLeft(_T('\t'));
	strGlobalItem.TrimRight(_T("\r\n"));
	// 项目信息结束
	if (strGlobalItem.CompareNoCase(_T("EndGlobalSection")) == 0)
		return;

	if (strGlobalItem.CompareNoCase(_T("EndGlobal")) == 0)
	{
		m_ContentType = CONTENT_TYPE_NONE;
		return;
	}

	// 获取激活项目信息
	DWORD dwLen = _tcslen(GLOBAL_PROJECT_ACTIVE);
	if (_tcsnicmp((LPCTSTR)strGlobalItem, GLOBAL_PROJECT_ACTIVE, dwLen) == 0)
	{
		CString strProjectGUID = strGlobalItem.Right(strGlobalItem.GetLength()-dwLen-1);	// 只取后面的项目GUID名称
		SetProjectActive(strProjectGUID, TRUE);
	}

	m_ContentType = CONTENT_TYPE_GLOBAL;
}

CString CSolution::GetSolutionFilePath()
{
	return m_strSolutionFile;
}

CString CSolution::GetSolutionFileVersion()
{
	return m_strSolutionFileVersion;
}

void CSolution::SetSolutionFileVersion(LPCTSTR lpszSolutionItemString)
{
	m_strSolutionFileVersion = _T("10.00");
}

CString CSolution::GetSolutionToolVersion()
{
	return m_strSolutionToolVersion;
}

void CSolution::SetSolutionToolVersion(LPCTSTR lpszSolutionItemString)
{
	m_strSolutionToolVersion = _T("2017");
}

void CSolution::SetProjectActiveStart()
{
	if (m_pSolutionTreeView == NULL)
		return;

	//	取消以前的启动项
	SetProjectActive(m_strActiveProject, FALSE);
	//	设置选中项为高亮
	m_pSolutionTreeView->SetSelectItemStart();
	// 设置新的启动项
	if (m_pSolutionTreeView->GetActiveProjectGUID(m_strActiveProject) != FALSE)
		SetProjectActive(m_strActiveProject, TRUE);
}

BOOL CSolution::GetSolutionProject(CProjectItemInfoList& SolutionInfo)
{
	SolutionInfo.RemoveAll();

	SolutionInfo.AddTail(&m_ProjectInfo);

	return SolutionInfo.IsEmpty() == FALSE;
}

void CSolution::SetProjectActive(LPCTSTR lpszProjectGUID, BOOL bActive)
{
	POSITION pos = m_ProjectInfo.GetHeadPosition();
	while(pos)
	{
		PROJECT_ITEM_INFO& ProjectItem = m_ProjectInfo.GetNext(pos);
		if (ProjectItem.strProjectGUID.CompareNoCase(lpszProjectGUID) == 0)
		{
			ProjectItem.bActive = bActive;
			m_strActiveProject = lpszProjectGUID;
		}
	}
}