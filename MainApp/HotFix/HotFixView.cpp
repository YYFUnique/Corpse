
// HotFixView.cpp : CHotFixView 类的实现
//

#include "stdafx.h"
#include "HotFix.h"

#include "HotFixDoc.h"
#include "HotFixView.h"

#include "LsCommon/CommonFunc.h"
#include "LsCommon/Wua/ScanSystemPatch.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define   WM_SCAN_SYSTEM_PATCH		(WM_USER + 0x100)
// CHotFixView

IMPLEMENT_DYNCREATE(CHotFixView, CListView)

BEGIN_MESSAGE_MAP(CHotFixView, CListView)
	ON_MESSAGE(WM_SCAN_SYSTEM_PATCH,CHotFixView::OnShowHotFix)
END_MESSAGE_MAP()

// CHotFixView 构造/析构

CHotFixView::CHotFixView()
{
	// TODO: 在此处添加构造代码

}

CHotFixView::~CHotFixView()
{
}

BOOL CHotFixView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CListView::PreCreateWindow(cs);
}

void CHotFixView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: 调用 GetListCtrl() 直接访问 ListView 的列表控件，
	//  从而可以用项填充 ListView。
	CListCtrl& ListCtrl = GetListCtrl();
	CHeaderCtrl* pHeader = ListCtrl.GetHeaderCtrl();
	if (pHeader == NULL)
	{
		int nIndex = 0;
		ListCtrl.InsertColumn(nIndex++,_T("编号"));
		ListCtrl.InsertColumn(nIndex++,_T("补丁名称"));
		ListCtrl.InsertColumn(nIndex++,_T("补丁描述"));
		ListCtrl.InsertColumn(nIndex++,_T("安装日期"));
		ListCtrl.InsertColumn(nIndex++,_T("详细信息"));
		ListCtrl.InsertColumn(nIndex++,_T("操作"));
	}

	SendMessage(WM_SCAN_SYSTEM_PATCH,0,0);
	AdjustListWidth(ListCtrl);
}


// CHotFixView 诊断

#ifdef _DEBUG
void CHotFixView::AssertValid() const
{
	CListView::AssertValid();
}

void CHotFixView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CHotFixDoc* CHotFixView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHotFixDoc)));
	return (CHotFixDoc*)m_pDocument;
}
#endif //_DEBUG


// CHotFixView 消息处理程序
LRESULT CHotFixView::OnShowHotFix(WPARAM wParam,LPARAM lParam)
{
	CScanSystemPatch ScanSystemPatch;
	CSystemPatchList SystemPatchList;
	ScanSystemPatch.ScanSystemPatchInfo(_T(""),SystemPatchList);
	return TRUE;
}