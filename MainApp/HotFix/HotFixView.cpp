
// HotFixView.cpp : CHotFixView ���ʵ��
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

// CHotFixView ����/����

CHotFixView::CHotFixView()
{
	// TODO: �ڴ˴���ӹ������

}

CHotFixView::~CHotFixView()
{
}

BOOL CHotFixView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CListView::PreCreateWindow(cs);
}

void CHotFixView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: ���� GetListCtrl() ֱ�ӷ��� ListView ���б�ؼ���
	//  �Ӷ������������ ListView��
	CListCtrl& ListCtrl = GetListCtrl();
	CHeaderCtrl* pHeader = ListCtrl.GetHeaderCtrl();
	if (pHeader == NULL)
	{
		int nIndex = 0;
		ListCtrl.InsertColumn(nIndex++,_T("���"));
		ListCtrl.InsertColumn(nIndex++,_T("��������"));
		ListCtrl.InsertColumn(nIndex++,_T("��������"));
		ListCtrl.InsertColumn(nIndex++,_T("��װ����"));
		ListCtrl.InsertColumn(nIndex++,_T("��ϸ��Ϣ"));
		ListCtrl.InsertColumn(nIndex++,_T("����"));
	}

	SendMessage(WM_SCAN_SYSTEM_PATCH,0,0);
	AdjustListWidth(ListCtrl);
}


// CHotFixView ���

#ifdef _DEBUG
void CHotFixView::AssertValid() const
{
	CListView::AssertValid();
}

void CHotFixView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CHotFixDoc* CHotFixView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHotFixDoc)));
	return (CHotFixDoc*)m_pDocument;
}
#endif //_DEBUG


// CHotFixView ��Ϣ�������
LRESULT CHotFixView::OnShowHotFix(WPARAM wParam,LPARAM lParam)
{
	CScanSystemPatch ScanSystemPatch;
	CSystemPatchList SystemPatchList;
	ScanSystemPatch.ScanSystemPatchInfo(_T(""),SystemPatchList);
	return TRUE;
}