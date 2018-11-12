#include "stdafx.h"
#include "Process.h"

#include "DllCore/Utils/ServiceControl.h"
#include "DllCore/Utils/Security.h"
#include "DllCore/File/FileSystemRedirecte.h"
#include "DllCore/Log/LogHelper.h"
#include "DllCore/Utils/TextTools.h"
#include "DllCore/Utils/FileTools.h"

CProcess::CProcess()
{
	SHFILEINFO ShFileInfo;
	memset(&ShFileInfo , 0 , sizeof(SHFILEINFO));
	SHGetFileInfo(_T("C:\\Windows\\System32\\wininit.exe") , NULL , &ShFileInfo , sizeof(SHFILEINFO) , SHGFI_ICON|SHGFI_SMALLICON);
	m_hAppIcon = ShFileInfo.hIcon;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_dwCpuCount = si.dwNumberOfProcessors;

	m_bLoad = FALSE;
	EnablePrivilege(SE_DEBUG_NAME);
}

CProcess::~CProcess()
{
	m_bLoad = TRUE;
	if (m_hAppIcon != NULL)
		DestroyIcon(m_hAppIcon);
}

DUI_BEGIN_MESSAGE_MAP(CProcess, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_TIMEREX, OnTimerEx)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE, OnItemActive)
DUI_END_MESSAGE_MAP()

void CProcess::SetPaintMagager(CPaintManagerUI *pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CProcess::OnRefresh(TNotifyUI& msg)
{
	OnTimerEx(msg);
	//CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Process"));
	//if (pList == NULL)
	//	return;

	//CProcessItemInfoList ProcessInfoList;
	//if (EnumProcessInfo(ProcessInfoList) == FALSE)
	//	return;

	//if (pList->GetCount())
	//	pList->RemoveAll();

	//TListInfoUI* pListInfo = pList->GetListInfo();
	////去掉DrawText函数中&被转义为_,比如&a会显示为'下划线a'
	//pList->SetItemTextStyle(pListInfo->uTextStyle & ~DT_NOPREFIX);

	//POSITION pos = ProcessInfoList.GetHeadPosition();
	//while(pos)
	//{
	//	const PROCESS_ITEM_INFO& ProcessInfo = ProcessInfoList.GetNext(pos);
	//	CreateProcessItem(pList, pListInfo, ProcessInfo);
	//}
}

void CProcess::OnTimerEx(TNotifyUI& msg)
{
	DWORD dwTickNow = GetTickCount();
	DWORD dwPeroid = dwTickNow - m_dwLastTick;
	m_dwLastTick = dwTickNow;
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Process"));
	if (pList == NULL)
		return;

	CProcessItemInfoList ProcessInfoList;
	if (EnumProcessInfo(ProcessInfoList) == FALSE)
		return;

	POSITION pos = ProcessInfoList.GetHeadPosition();
	while(pos)
	{
		BOOL bFindProcess = FALSE;
		const PROCESS_ITEM_INFO& ProcessItem = ProcessInfoList.GetNext(pos);
		for (int n=0; n<pList->GetCount(); ++n)
		{
			CFileListItemUI* pProcess = (CFileListItemUI*)pList->GetItemAt(n);

			DWORD dwProcessId = (DWORD)_ttoi(pProcess->GetSubControlText(_T("Pid")));
			if (ProcessItem.dwProcessId != dwProcessId)
				continue;

			bFindProcess = TRUE;

			// 修改内存使用情况
			CDuiString strMemInfo = pProcess->GetSubControlText(_T("MemInfo"));
			TCHAR szMemInfo[20];
			StrFormatKBSize(ProcessItem.dwMemUse, szMemInfo, _countof(szMemInfo));
			
			if (strMemInfo != szMemInfo)
				pProcess->SetSubControlText(_T("MemInfo"), szMemInfo);

			// 修改CPU使用情况
			DWORD dwCPULast = pProcess->GetTag();
			DWORD dwCPUNow = (DWORD)(ProcessItem.ullProcessUseTime/10000);
			if (dwCPULast != dwCPUNow)
			{
				pProcess->SetTag((UINT_PTR)dwCPUNow);
				TCHAR szCPUInfo[50];
				// 由于是多核心CPU，需要分摊计算到每个核心中
				CString strCPUInfo = pProcess->GetSubControlText(_T("CpuInfo"));
				DWORD dwPercent = (dwCPUNow - dwCPULast)*100 / (dwPeroid*m_dwCpuCount);
				if (dwPercent > 99)
					dwPercent = 99;
				_stprintf_s(szCPUInfo, _countof(szCPUInfo), _T("%02u"), dwPercent);
				// StrFromTimeInterval(szCPUInfo, _countof(szCPUInfo), ProcessItem.ullProcessUseTime/10000,6);
				//	StrFromTimeIntervalEx(szCPUInfo, _countof(szCPUInfo), ProcessItem.ullProcessUseTime/10000, 6);
				if (_tcslen(szCPUInfo) < 3)
					pProcess->SetSubControlText(_T("CpuInfo"), szCPUInfo);
			}		

			// 修改线程情况
			CDuiString strThreadInfo = pProcess->GetSubControlText(_T("TheadInfo"));
			TCHAR szThreadInfo[20];
			StrFormatNumber(ProcessItem.dwThread, szThreadInfo, _countof(szThreadInfo));

			if (strThreadInfo != szThreadInfo)
				pProcess->SetSubControlText(_T("TheadInfo"), szThreadInfo);

			// 修改句柄情况
			if (ProcessItem.dwHandle != 0)
			{
				CDuiString strHandleInfo = pProcess->GetSubControlText(_T("HandleInfo"));
				TCHAR szHandleInfo[20];
				StrFormatNumber(ProcessItem.dwHandle, szHandleInfo, _countof(szHandleInfo));

				if (strHandleInfo != szHandleInfo)
					pProcess->SetSubControlText(_T("HandleInfo"), szHandleInfo);
			}

			break;
		}

		CFileSystemRedirection FileSystemRedirected(TRUE);

		if (bFindProcess == FALSE)
		{
			TListInfoUI* pListInfo = pList->GetListInfo();
			//去掉DrawText函数中&被转义为_,比如&a会显示为'下划线a'
			pList->SetItemTextStyle(pListInfo->uTextStyle & ~DT_NOPREFIX);
			CreateProcessItem(pList, pListInfo, ProcessItem);
		}
	}
}

void CProcess::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	OnRefresh(msg);
}

void CProcess::OnItemActive(TNotifyUI& msg)
{

}

void CProcess::CreateProcessItem(CListUI* pList, const TListInfoUI* pListInfo, const PROCESS_ITEM_INFO& ProcessInfo)
{
	CFileListItemUI* pFileItem = NULL;
	if (m_DialogBuilder.GetMarkup()->IsValid() == false)
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(PROCESS_LIST_ITEM_INFO, 0, &m_RootBuilder, m_pPaintManager);
	else
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

	pList->Add(pFileItem);
	pFileItem->SetTag((UINT_PTR)ProcessInfo.ullProcessUseTime/10000);
	pFileItem->SetFixedHeight(27);

	CPictureUI* pPicIcon = (CPictureUI*)pFileItem->FindSubControl(_T("Pic"));
	pPicIcon->SetVisible(true);

	if (ProcessInfo.dwProcessId == 0 || ProcessInfo.dwProcessId == 4)
		pPicIcon->SetIcon(m_hAppIcon);
	else
	{
		SHFILEINFO ShFileInfo;
		SHGetFileInfo(ProcessInfo.strProcessPath, NULL, &ShFileInfo, sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON);

		if (ShFileInfo.hIcon != NULL)
		{
			CPictureUI* pPicIcon = (CPictureUI*)pFileItem->FindSubControl(_T("Pic"));
			pPicIcon->SetIcon(ShFileInfo.hIcon);
			DestroyIcon(ShFileInfo.hIcon);
		}
		else
			pPicIcon->SetIcon(m_hAppIcon);
	}
	
	// 获取进程名
	CLabelUI* pLabelTitle = (CLabelUI*)pFileItem->FindSubControl(_T("ProcessName"));
	pLabelTitle->SetText(ProcessInfo.strProcessName);
	pLabelTitle->SetFont(pListInfo->nFont);
	pLabelTitle->SetForeColor(pListInfo->dwTextColor);
	pLabelTitle->AppendTextStyle(DT_END_ELLIPSIS);

	// 获取进程ID
	CLabelUI* pPid = (CLabelUI*)pFileItem->FindSubControl(_T("Pid"));
	CDuiString strTipInfo;
	strTipInfo.Format(_T("%d"), ProcessInfo.dwProcessId);
	pPid->SetText(strTipInfo);
	pPid->SetFont(pListInfo->nFont);
	pPid->SetForeColor(pListInfo->dwTextColor);
	pPid->AppendTextStyle(DT_END_ELLIPSIS);
	pPid->SetAttribute(_T("align"), _T("right"));

	// 获取进程用户名
	CLabelUI* pUserName = (CLabelUI*)pFileItem->FindSubControl(_T("UserName"));
	pUserName->SetText(ProcessInfo.strUserName);
	pUserName->SetFont(pListInfo->nFont);
	pUserName->SetForeColor(pListInfo->dwTextColor);
	pUserName->AppendTextStyle(DT_END_ELLIPSIS);
	pUserName->SetAttribute(_T("align"), _T("right"));

	// 获取应用占用内存
	CLabelUI* pMemInfo = (CLabelUI*)pFileItem->FindSubControl(_T("MemInfo"));
	TCHAR szMemUse[20];
	StrFormatKBSize(ProcessInfo.dwMemUse, szMemUse, _countof(szMemUse));
	pMemInfo->SetText(szMemUse);
	pMemInfo->SetFont(pListInfo->nFont);
	pMemInfo->SetForeColor(pListInfo->dwTextColor);
	pMemInfo->AppendTextStyle(DT_END_ELLIPSIS);

	// 获取应用CPU使用率
	CLabelUI* pCpuInfo = (CLabelUI*)pFileItem->FindSubControl(_T("CpuInfo"));
	pCpuInfo->SetText(_T("00"));
	pCpuInfo->SetFont(pListInfo->nFont);
	pCpuInfo->SetForeColor(pListInfo->dwTextColor);
	pCpuInfo->AppendTextStyle(DT_END_ELLIPSIS);

	// 获取应用线程数量
	CLabelUI* pTheadInfo = (CLabelUI*)pFileItem->FindSubControl(_T("TheadInfo"));
	TCHAR szThread[20];
	StrFormatNumber(ProcessInfo.dwThread, szThread, _countof(szThread));
	pTheadInfo->SetText(szThread);
	pTheadInfo->SetFont(pListInfo->nFont);
	pTheadInfo->SetForeColor(pListInfo->dwTextColor);
	pTheadInfo->AppendTextStyle(DT_END_ELLIPSIS);

	// 获取应用打开句柄数量
	if (ProcessInfo.dwHandle != 0)
	{
		CLabelUI* pHandleInfo = (CLabelUI*)pFileItem->FindSubControl(_T("HandleInfo"));
		TCHAR szHandle[20];
		StrFormatNumber(ProcessInfo.dwHandle, szHandle, _countof(szHandle));
		pHandleInfo->SetText(szHandle);
		pHandleInfo->SetFont(pListInfo->nFont);
		pHandleInfo->SetForeColor(pListInfo->dwTextColor);
		pHandleInfo->AppendTextStyle(DT_END_ELLIPSIS);
	}

	// 获取文件描述
	CLabelUI* pDesc = (CLabelUI*)pFileItem->FindSubControl(_T("Desc"));
	CString strFileDesc = (LPCTSTR)m_strFileDescInfo.Find(ProcessInfo.strProcessName);
	if (strFileDesc.IsEmpty())
	{
		if (ProcessInfo.dwProcessId == 0)
			strFileDesc = _T("处理器空闲时间百分比");
		else
			GetFileVersionEx(ProcessInfo.strProcessPath, FILE_VERSION_FileDescription, strFileDesc);
		if (strFileDesc.IsEmpty() != FALSE)
			strFileDesc = ProcessInfo.strProcessName;

		DWORD dwLen = (strFileDesc.GetLength() + 1) * sizeof(TCHAR);

		LPTSTR lpszFileDesc = new TCHAR[dwLen];
		RtlCopyMemory(lpszFileDesc, strFileDesc, dwLen);
		m_strFileDescInfo.Insert(ProcessInfo.strProcessName, lpszFileDesc);
	}
	pDesc->SetText(strFileDesc);
	pDesc->SetFont(pListInfo->nFont);
	pDesc->SetForeColor(pListInfo->dwTextColor);
	pDesc->AppendTextStyle(DT_END_ELLIPSIS|DT_NOPREFIX);
}
