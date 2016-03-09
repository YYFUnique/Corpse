
// CorpseToolsView.cpp : CCorpseToolsView ���ʵ��
//

#include "stdafx.h"
#include "CorpseTools.h"

#include "CorpseToolsDoc.h"
#include "CorpseToolsView.h"

#include "Group/Debug/CleanProject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCorpseToolsView

IMPLEMENT_DYNCREATE(CCorpseToolsView, CListView)

BEGIN_MESSAGE_MAP(CCorpseToolsView, CListView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CCorpseToolsView::OnNMDblclk)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CCorpseToolsView ����/����

CCorpseToolsView::CCorpseToolsView()
{
	// TODO: �ڴ˴���ӹ������
	//m_pToolItemList = new CToolItemList;
}

CCorpseToolsView::~CCorpseToolsView()
{
	//if (m_pToolItemList != NULL)
	//{
	//	delete m_pToolItemList;
	//	m_pToolItemList = NULL;
	//}
}

BOOL CCorpseToolsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CListView::PreCreateWindow(cs);
}

void CCorpseToolsView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: ���� GetListCtrl() ֱ�ӷ��� ListView ���б�ؼ���
	//  �Ӷ������������ ListView��
	CListCtrl& ListCtrl = GetListCtrl();

	m_pToolItemList.RemoveAll();
	m_pToolItemList.AddTail(new CCleanProject);

	m_IconImageList.DeleteImageList();
	m_IconImageList.Create(32 , 32 , ILC_COLOR32 | ILC_MASK , 10 , 1);

	ListCtrl.SetImageList(&m_IconImageList,LVSIL_NORMAL);

	POSITION pos = m_pToolItemList.GetHeadPosition();
	int nItem=0;
	while(pos)
	{
		CToolItemBase* pToolItemBase = m_pToolItemList.GetNext(pos);

		HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(pToolItemBase->GetIconResourceId()));
		int nImageIndex = m_IconImageList.Add(hIcon);

		ListCtrl.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE,nItem++,pToolItemBase->GetItemName(),NULL,NULL,nImageIndex,(LPARAM)pToolItemBase);
	}
}


// CCorpseToolsView ���

#ifdef _DEBUG
void CCorpseToolsView::AssertValid() const
{
	CListView::AssertValid();
}

void CCorpseToolsView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CCorpseToolsDoc* CCorpseToolsView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCorpseToolsDoc)));
	return (CCorpseToolsDoc*)m_pDocument;
}
#endif //_DEBUG


// CCorpseToolsView ��Ϣ�������
void CCorpseToolsView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
 {
	 LPNMITEMACTIVATE pNmItem = (LPNMITEMACTIVATE)pNMHDR;

	 CListCtrl& ListCtrl = GetListCtrl();
	 POSITION pos = ListCtrl.GetFirstSelectedItemPosition();
	 if (pos == NULL)
		 return;

	 int nItem = ListCtrl.GetNextSelectedItem(pos);
	 CToolItemBase* pToolItemBase = (CToolItemBase*)ListCtrl.GetItemData(nItem);
	 ASSERT(pToolItemBase);

	 pToolItemBase->OnLButtonDbClick();
 }

void CCorpseToolsView::OnContextMenu(CWnd* pWnd, CPoint pt)
{
	CListCtrl& ListCtrl = GetListCtrl();
	POSITION pos = ListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return;


	int nItem = ListCtrl.GetNextSelectedItem(pos);
	CToolItemBase* pToolItemBase = (CToolItemBase*)ListCtrl.GetItemData(nItem);
	ASSERT(pToolItemBase);

	CMenu Menu,*pMenu;
	Menu.LoadMenu(IDR_TOOL_ITEM_MENU);
	pMenu=Menu.GetSubMenu(pToolItemBase->GetSubItemMenu());

	POINT point;
	GetCursorPos(&point);
	DWORD dwMenuId=pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD,point.x,point.y,this,0);
	if (dwMenuId == 0)//�û�δѡ��
		return;

	BeginWaitCursor();
	//bSuccess = pToolItemBase->OnMenuCheckSate(dwMenuId);
	//pToolItemBase->OnMenuInvoke(dwMenuId);

	EndWaitCursor();
}

void CCorpseToolsView::OnDestroy()
{
	ClearToolItem();
}

BOOL CCorpseToolsView::ClearToolItem()
{
	CListCtrl& ListCtrl = GetListCtrl();
	int nItemTotal = ListCtrl.GetItemCount();
	CToolItemBase* pToolItem;
	for (int n=0;n<nItemTotal;++n)
	{
		pToolItem = (CToolItemBase*)ListCtrl.GetItemData(n);
		delete pToolItem;
		pToolItem = NULL;
	}
	return TRUE;
}