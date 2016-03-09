// TabCtrlProcessInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "../TaskMar.h"
#include "TabCtrlProcessInfo.h"
#include "LsCommon/CommonFunc.h"
#include "ProcessInfo.h"
#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")
// CTabCtrlProcessInfo 对话框

IMPLEMENT_DYNAMIC(CTabCtrlProcessInfo, CDialog)

CTabCtrlProcessInfo::CTabCtrlProcessInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CTabCtrlProcessInfo::IDD, pParent)
{
	m_pProcessInfo = NULL;
}

CTabCtrlProcessInfo::~CTabCtrlProcessInfo()
{
	if (m_pProcessInfo != NULL)
	{
		delete m_pProcessInfo;
		m_pProcessInfo = NULL;
	}
}

void CTabCtrlProcessInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CTabCtrlProcessInfo, CDialog)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_NOTIFY_EX(NM_RCLICK,IDC_LIST_INFO,&CTabCtrlProcessInfo::OnRClick)
	ON_COMMAND(ID_LOOKUP_THREAD_FILE, &CTabCtrlProcessInfo::OnLookupThreadFile)
END_MESSAGE_MAP()


// CTabCtrlProcessInfo 消息处理程序

BOOL CTabCtrlProcessInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowTheme(m_ListCtrl.m_hWnd,L"Explorer",NULL);
	m_ListCtrl.ModifyStyle(0,LVS_REPORT);
	
	m_ListCtrl.SetExtendedStyle(LVS_EX_LABELTIP|LVS_EX_DOUBLEBUFFER|LVS_EX_SINGLEROW|LVS_EX_FULLROWSELECT);
	CStringArray strColumn;
	m_pProcessInfo = CProcessInfo::Create(m_ListId);
	if (m_pProcessInfo->GetListColumn(strColumn) == FALSE)
		return FALSE;
	
	if (m_pProcessInfo->IsShowImageList() == TRUE)
		m_ListCtrl.SetImageList(m_pProcessInfo->GetImageListPtr(),LVSIL_SMALL);

	for(int i=0;i<strColumn.GetSize();++i)
	{
		CString& strHeaderItem = strColumn.ElementAt(i);
		m_ListCtrl.InsertColumn(i,strHeaderItem);
	}

	AdjustListWidth(m_ListCtrl);
	return TRUE;
}

BOOL CTabCtrlProcessInfo::Create(CWnd* pParent,DWORD dwListId)
{
	ASSERT(pParent);
	m_ListId = dwListId;
	return CDialog::Create(IDD,pParent);
}

void CTabCtrlProcessInfo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (::IsWindow(m_ListCtrl) == FALSE)
		return;
	
	CRect Rect;
	GetParent()->GetClientRect(&Rect);

	Rect.bottom -= 21;
	Rect.right -=10;
	m_ListCtrl.MoveWindow(Rect,TRUE);
}

void CTabCtrlProcessInfo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
		case VK_ESCAPE:
			break;
		case VK_RETURN:
			break;
		default:
			CDialog::OnKeyDown(nChar,nRepCnt,nFlags);
	}
}

BOOL CTabCtrlProcessInfo::OnRClick(UINT nID,NMHDR* pNMHdr,LPARAM* lResult)
{
	NMLISTVIEW* pLVCD = (NMLISTVIEW*)pNMHdr;
	if (pLVCD->iItem < 0 )
		return FALSE;

	CMenu MenuTabInfo,*pMenuTabInfo;
	MenuTabInfo.LoadMenu(IDR_MENU_PROCESS_INFO);
	pMenuTabInfo = MenuTabInfo.GetSubMenu(m_pProcessInfo->GetSubMenu());

	SetForegroundWindow();

	CPoint pt;
	GetCursorPos(&pt);
	pMenuTabInfo->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);

	return TRUE;
}

BOOL CTabCtrlProcessInfo::ShowTabCtrlInfo()
{
	ASSERT(m_pProcessInfo);
	m_pProcessInfo->ShowProcessInfo(m_ListCtrl);
	AdjustListWidth(m_ListCtrl);
	return TRUE;
}

UINT CTabCtrlProcessInfo::GetTabIcon()
{
	ASSERT(m_pProcessInfo);
	return m_pProcessInfo->GetTabCtrlID();
}

void CTabCtrlProcessInfo::OnLookupThreadFile()
{
	// TODO: 在此添加命令处理程序代码
	m_pProcessInfo->OnRButtonDown(ID_LOOKUP_THREAD_FILE);
}
