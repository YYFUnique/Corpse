#include "stdafx.h"
#include "ProcessHandleInfo.h"
#include "LsCommon/NtDll.h"

CProcessHandleInfo::CProcessHandleInfo()
{
	m_nSubMenuID = PROCESS_HANDLE;
}

// CProcessHandleInfo::~CProcessHandleInfo()
// {
// 
// }

BOOL CProcessHandleInfo::GetListColumn(CStringArray& strListColumn)
{
	strListColumn.RemoveAll();

	strListColumn.Add(_T("句柄"));
	strListColumn.Add(_T("类型"));
	strListColumn.Add(_T("权限"));
	strListColumn.Add(_T("名称"));
	return TRUE;
}

BOOL CProcessHandleInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	CTaskMarView* pView = (CTaskMarView*)GetProcessListView();
	ASSERT(pView);
	DWORD dwPid = pView->GetProcessID();

	ListCtrl.SetRedraw(FALSE);
	ListCtrl.DeleteAllItems();

	HANDLE hProcess = NULL;
	if (GetProcessHandle(dwPid,hProcess) == FALSE)
		return FALSE;

	CSystemHandleInfoList SystemHandleInfoList;
	SystemHandleInfoList.RemoveAll();
	if (GetSystemHandleInfo(dwPid,SystemHandleInfoList) == FALSE)
		return FALSE;

	POSITION pos = SystemHandleInfoList.GetHeadPosition();
	while(pos)
	{
		SYSTEM_HANDLE_INFORMATION& SystemHandle = SystemHandleInfoList.GetNext(pos);

		HANDLE DumpHandle;
		DuplicateHandle(hProcess,(HANDLE)SystemHandle.Handle,GetCurrentProcess(),&DumpHandle,
			DUPLICATE_SAME_ACCESS,FALSE,DUPLICATE_SAME_ACCESS);

		int nItem = ListCtrl.InsertItem(LVIF_PARAM|LVIF_TEXT,0x7FFFFFFF,FormatNumToHexString(SystemHandle.Handle),0,0,0,(LPARAM)DumpHandle);
		int nSubItem = 0;
		CString strHandleType;

		strHandleType.Format(_T("%s[Type=%d]"),GetHandleDes(DumpHandle,ObjectTypeInformation),SystemHandle.ObjectTypeNumber);
		ListCtrl.SetItemText(nItem,++nSubItem,strHandleType);
		ListCtrl.SetItemText(nItem,++nSubItem,FormatNumToHexString(SystemHandle.GrantedAccess));
		ListCtrl.SetItemText(nItem,++nSubItem,GetHandleDes(DumpHandle,ObjectNameInformation));
		CloseHandle(DumpHandle);
	}

	ListCtrl.SetRedraw(TRUE);
	CloseHandle(hProcess);
	return TRUE;
}

BOOL CProcessHandleInfo::OnRButtonDown(UINT nID)
{
	return TRUE;
}