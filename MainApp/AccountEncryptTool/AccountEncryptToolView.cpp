
// AccountEncryptToolView.cpp : CAccountEncryptToolView 类的实现
//

#include "stdafx.h"
#include "AccountEncryptTool.h"

#include "AccountEncryptToolDoc.h"
#include "AccountEncryptToolView.h"
#include "LsCommon/CommonFunc.h"

#include <uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAccountEncryptToolView

IMPLEMENT_DYNCREATE(CAccountEncryptToolView, CListView)

BEGIN_MESSAGE_MAP(CAccountEncryptToolView, CListView)
END_MESSAGE_MAP()

// CAccountEncryptToolView 构造/析构

CAccountEncryptToolView::CAccountEncryptToolView()
{
	// TODO: 在此处添加构造代码

}

CAccountEncryptToolView::~CAccountEncryptToolView()
{
}

BOOL CAccountEncryptToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CListView::PreCreateWindow(cs);
}

void CAccountEncryptToolView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: 调用 GetListCtrl() 直接访问 ListView 的列表控件，
	//  从而可以用项填充 ListView。
	CListCtrl& ListCtrl = GetListCtrl();
	CHeaderCtrl* pHeader = ListCtrl.GetHeaderCtrl();
	if (pHeader == NULL)
	{
		SetWindowTheme(m_hWnd,L"Explorer",NULL);
		ModifyStyle(LVS_TYPEMASK , LVS_REPORT , 0);
		ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);
		int nIndex=0;
		ListCtrl.InsertColumn(nIndex++,_T("编号"));
		ListCtrl.InsertColumn(nIndex++,_T("名称"));
		ListCtrl.InsertColumn(nIndex++,_T("扩展信息"));
		ListCtrl.EnableGroupView(TRUE);
	}
	AdjustListWidth();
}

// CAccountEncryptToolView 诊断

#ifdef _DEBUG
void CAccountEncryptToolView::AssertValid() const
{
	CListView::AssertValid();
}

void CAccountEncryptToolView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CAccountEncryptToolDoc* CAccountEncryptToolView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAccountEncryptToolDoc)));
	return (CAccountEncryptToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CAccountEncryptToolView 消息处理程序
BOOL CAccountEncryptToolView::AddGroup(LPCTSTR lpszGroupName,int nGroupId)
{
	return InsertGroup(GetListCtrl(),lpszGroupName,nGroupId);
}

BOOL CAccountEncryptToolView::AddItemToGroup(ACCOUNT_INFO& AccountInfo)
{
	CListCtrl& ListCtrl = GetListCtrl();
	LVITEM lvItem;
	memset(&lvItem,0,sizeof(lvItem));
	lvItem.iItem = 0x7FFFFFFF;
	lvItem.iGroupId = AccountInfo.dwGroupId;
	lvItem.pszText = _T("0");
	lvItem.mask = LVIF_GROUPID|LVIF_TEXT;
	int nItem = ListCtrl.InsertItem(&lvItem);
	ListCtrl.SetItemText(nItem,1,AccountInfo.lpszItmeName);

	return TRUE;
}

BOOL CAccountEncryptToolView::AdjustListWidth()
{
	return ::AdjustListWidth(GetListCtrl());
}