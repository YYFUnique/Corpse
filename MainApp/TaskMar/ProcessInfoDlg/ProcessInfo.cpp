#include "stdafx.h"
#include "ProcessInfo.h"

#include "ProcessDetailInfo.h"
#include "ProcessModuleInfo.h"
#include "ProcessWinInfo.h"
#include "ProcessHandleInfo.h"
#include "ProcessThreadInfo.h"
#include "ProcessServiceInfo.h"
#include "ProcessStartInfo.h"
#include "ProcessMemInfo.h"

CProcessInfo::CProcessInfo()
	: m_TabCtrlID(0)
	, m_nSubMenuID(0)
	, m_bIsAddList(FALSE)
	, bShowImageList(FALSE)
{
	m_ModuleImg.DeleteImageList();
	m_ModuleImg.Create(16,16,ILC_COLOR32|ILC_MASK,10,5);
}

CProcessInfo::~CProcessInfo()
{
	m_ModuleImg.DeleteImageList();
}

UINT CProcessInfo::GetTabCtrlID()
{
	return m_TabCtrlID;
}

CProcessInfo* CProcessInfo::Create(DWORD dwListType)
{
	CProcessInfo* pProcessInfo = NULL;
	switch(dwListType)
	{
		case PROCESS_DETAIL_INFO:
				pProcessInfo = new CProcessDetailInfo;
			break;
		case	PROCESS_MODULE	:
				pProcessInfo = new CProcessModuleInfo;
			break;
		case PROCESS_WINDOWS:
				pProcessInfo = new CProcessWinInfo;
			break;
		case	PROCESS_MEMROY:
				pProcessInfo = new CProcessMemInfo;
			break;
		case	PROCESS_HANDLE	:
				pProcessInfo = new CProcessHandleInfo;
			break;
		case	PROCESS_THREAD:
				pProcessInfo = new CProcessThreadInfo;
			break;
		/*case	PROCESS_SERVICE:
				pProcessInfo = new CProcessServiceInfo;
			break;
		case	PROCESS_STARTTYPE	:
				pProcessInfo = new CProcessStartInfo;
			break;*/
	}

	return pProcessInfo;
}

BOOL CProcessInfo::ShowProcessInfo(CListCtrl& ListCtrl)
{
	return TRUE;
}

CView* CProcessInfo::GetProcessListView()
{
	CFrameWnd* pMainWnd = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	ASSERT(pMainWnd);
	CTaskMarDoc* pDocument = (CTaskMarDoc*)pMainWnd->GetActiveDocument();
	ASSERT(pDocument);

	return	pDocument->GetView(RUNTIME_CLASS(CTaskMarView));
}

CImageList* CProcessInfo::GetImageListPtr()
{
	return &m_ModuleImg;
}

BOOL CProcessInfo::IsShowImageList()
{
	return bShowImageList;
}

UINT CProcessInfo::GetSubMenu()
{
	return m_nSubMenuID;
}