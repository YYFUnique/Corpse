// IEHistoryModule.cpp : 实现文件
//

#include "stdafx.h"
#include "IEHistory.h"
#include "IEHistoryModule.h"
#include "IEHistoryDoc.h"
#include "IEHistoryView.h"

#include "LsCommon/OsInfo.h"
#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")
// IEHistoryModule

IMPLEMENT_DYNCREATE(CIEHistoryModule, CTreeView)

CIEHistoryModule::CIEHistoryModule()
{

}

CIEHistoryModule::~CIEHistoryModule()
{
}

BEGIN_MESSAGE_MAP(CIEHistoryModule, CTreeView)
	ON_NOTIFY_REFLECT(NM_CLICK,&CIEHistoryModule::OnSelectItem)
	ON_NOTIFY_REFLECT(NM_RCLICK,&CIEHistoryModule::OnItemRButton)
END_MESSAGE_MAP()


// IEHistoryModule 诊断

#ifdef _DEBUG
void CIEHistoryModule::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CIEHistoryModule::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// IEHistoryModule 消息处理程序
BOOL CIEHistoryModule::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT;
	return CTreeView::PreCreateWindow(cs);
}

void CIEHistoryModule::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	SetWindowTheme(m_hWnd,L"Explorer",NULL);

	CTreeCtrl& TreeCtrl = GetTreeCtrl();


	HTREEITEM hRoot = TreeCtrl.InsertItem(TVIF_PARAM|TVIF_TEXT,GetComputerName(),0,0,0,0,MODULE_TYPE_Undefined,NULL,NULL);
	TreeCtrl.SelectItem(hRoot);

	LPCTSTR lpszItem[] = {_T("历史记录"),_T("浏览器地址栏缓存"),_T("Cookies"),_T("IE缓存"),_T("资源管理器地址栏缓存"),_T("命令行使用列表")};
	for (int i=0;i<_countof(lpszItem);++i)
		TreeCtrl.InsertItem(TVIF_PARAM|TVIF_TEXT,lpszItem[i],0,0,0,0,i+1,hRoot,NULL);

	TreeCtrl.Expand(hRoot,TVE_EXPAND);
}

void CIEHistoryModule::OnSelectItem(NMHDR* pNMHDR,LRESULT* pLRESULT)
{
	NMTREEVIEW* pTreeView = (NMTREEVIEW*)pNMHDR;

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	CPoint pt;
	GetCursorPos(&pt);
	TreeCtrl.ScreenToClient(&pt);
	UINT uFlag;
	HTREEITEM ht = TreeCtrl.HitTest(pt,&uFlag);
	if (ht != NULL && TVHT_ONITEM & uFlag)
	{
		MODULE_TYPE ModuleType= (MODULE_TYPE)TreeCtrl.GetItemData(ht);
		CIEHistoryDoc* pDoc = (CIEHistoryDoc*)GetDocument();
		CIEHistoryView* pView = (CIEHistoryView*)(pDoc->GetView(RUNTIME_CLASS(CIEHistoryView)));

		pView->SetListContent(ModuleType);
	}
}

void CIEHistoryModule::OnItemRButton(NMHDR* pNMHDR,LRESULT* pLRESULT)
{

}