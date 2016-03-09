
// LeftView.cpp : CLeftView ���ʵ��
//

#include "stdafx.h"
#include "PETool.h"

#include "PEToolDoc.h"
#include "LeftView.h"
#include "PEToolView.h"
#include "PELibrary.h"

#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_ITEMCHANGED,OnItemChanged)
END_MESSAGE_MAP()


// CLeftView ����/����

CLeftView::CLeftView()
{
	// TODO: �ڴ˴���ӹ������
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT;

	return CTreeView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: ���� GetTreeCtrl() ֱ�ӷ��� TreeView �����ؼ���
	//  �Ӷ������������ TreeView��
	SetWindowTheme(m_hWnd,L"Explorer",NULL);
	m_pPEToolView = (CPEToolView*)GetDocument()->GetView(RUNTIME_CLASS(CPEToolView));
}


// CLeftView ���

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CPEToolDoc* CLeftView::GetDocument() // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPEToolDoc)));
	return (CPEToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView ��Ϣ�������
void CLeftView::OnItemChanged(NMHDR* pNMHdr,LPARAM* lResult)
{
	NMTVITEMCHANGE* pNMItemChanged = (NMTVITEMCHANGE *)pNMHdr;
	if (pNMItemChanged->uStateNew == TVIS_SELECTED)
	{
		CTreeCtrl& TreeCtrl = GetTreeCtrl();
		HTREEITEM hTreeItem = pNMItemChanged->hItem;

 		if (TreeCtrl.ItemHasChildren(hTreeItem) == FALSE)
 			m_pPEToolView->OnShowItemData((ITEM_DATA_INFO)TreeCtrl.GetItemData(hTreeItem),m_pPELibrary);
	}
	*lResult = FALSE;
}

BOOL CLeftView::InsertPEHeaderInfo(CPELibrary* pPELibrary)
{
	BOOL bSuccess = FALSE;

	do 
	{
 		CTreeCtrl& TreeCtrl = GetTreeCtrl();
 		HTREEITEM hRoot = TreeCtrl.InsertItem(pPELibrary->GetFileName(),NULL,NULL);
		HTREEITEM hSection = TreeCtrl.InsertItem(_T("Section"),hRoot,TVI_LAST);
		
		TreeCtrl.InsertItem(TVIF_TEXT|TVIF_PARAM,_T("Dos Header"),0,0,0,0,(LPARAM)ITEM_DATA_INFO_DOS_HEADER,hRoot,TVI_LAST);
		TreeCtrl.InsertItem(TVIF_TEXT|TVIF_PARAM,_T("File Header"),0,0,0,0,(LPARAM)ITEM_DATA_INFO_FILE_HEADER,hRoot,TVI_LAST);
		TreeCtrl.InsertItem(TVIF_TEXT|TVIF_PARAM,_T("Section"),0,0,0,0,(LPARAM)ITEM_DATA_INFO_DOS_HEADER,hSection,TVI_LAST);
		
		TreeCtrl.Expand(hRoot,TVE_TOGGLE);

		m_pPELibrary = pPELibrary;
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}