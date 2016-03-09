#include "stdafx.h"
#include "ProcessModuleInfo.h"

CProcessModuleInfo::CProcessModuleInfo()
{
	bShowImageList = TRUE;
	m_TabCtrlID = IDI_ICON_DEFAULT;
	m_nSubMenuID = PROCESS_MODULE;
}

// CProcessModuleInfo::~CProcessModuleInfo()
// {
// 
// }

BOOL CProcessModuleInfo::GetListColumn(CStringArray& strListColumn)
{
	strListColumn.RemoveAll();

	strListColumn.Add(_T("模块名称"));
	strListColumn.Add(_T("地址范围"));

	strListColumn.Add(_T("大小"));
	strListColumn.Add(_T("修改时间"));
	strListColumn.Add(_T("公司"));
	strListColumn.Add(_T("文件版本"));
	strListColumn.Add(_T("文件描述"));
	strListColumn.Add(_T("文件路径"));
	return TRUE;
}

BOOL CProcessModuleInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	ASSERT(ListCtrl);

	CTaskMarView* pView = (CTaskMarView*)GetProcessListView();
	ASSERT(pView);
	DWORD dwPid = pView->GetProcessID();
	HANDLE hProcess;
	GetProcessHandle(dwPid,hProcess);
	if (hProcess == NULL)
		return FALSE;

	ListCtrl.SetRedraw(FALSE);
	ListCtrl.DeleteAllItems();
	HMODULE hModule[1024];
	DWORD dwNeeded;

	if( EnumProcessModules(hProcess, hModule, sizeof(hModule), &dwNeeded))
	{
		CString strModuleName;
		for ( UINT i = 0; i < (dwNeeded / sizeof(HMODULE)); i++ )
		{
			TCHAR szModName[MAX_PATH];
			if ( GetModuleFileNameEx(hProcess, hModule[i], szModName,sizeof(szModName)/sizeof(TCHAR)))
			{
				strModuleName = PathFindFileName(szModName);
				int nIconIndex = GetModuleFileIcon(szModName,&m_ModuleImg);
				int nIndex = ListCtrl.InsertItem(0x7FFFFFFF,strModuleName,nIconIndex);
				int nSubItem = 0;
				//地址空间
				MODULEINFO ModuleInfo;
				DWORD dwSize = sizeof(MODULEINFO);
				GetModuleInformation(hProcess,hModule[i],&ModuleInfo,dwSize);
				
				ListCtrl.SetItemText(nIndex,++nSubItem,GetModuleAddressRange(ModuleInfo));
				ListCtrl.SetItemText(nIndex,++nSubItem,FormatNumSizeToStringSize(GetFileSizeByName(szModName)));
				ListCtrl.SetItemText(nIndex,++nSubItem,FormatTimeToString(GetFileLastModifyTime(szModName)));
				CString strFileVersionInfo;

				LPCTSTR lpszFileVersion[] = {FILE_VERSION_CompanyName,FILE_VERSION_FileVersion,FILE_VERSION_FileDescription};
				for(int j=0;j<_countof(lpszFileVersion);++j)
				{
					GetFileVersionEx(szModName,lpszFileVersion[j],strFileVersionInfo);
					ListCtrl.SetItemText(nIndex,++nSubItem,strFileVersionInfo);
				}

				ListCtrl.SetItemText(nIndex,++nSubItem,szModName);
			}
		}
	}

	ListCtrl.SetRedraw(TRUE);
	CloseHandle(hProcess);
	
	return TRUE;
}

BOOL CProcessModuleInfo::OnRButtonDown(UINT nID)
{
	return TRUE;
}