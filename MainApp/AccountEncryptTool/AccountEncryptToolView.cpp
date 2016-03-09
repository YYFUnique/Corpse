
// AccountEncryptToolView.cpp : CAccountEncryptToolView ���ʵ��
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

// CAccountEncryptToolView ����/����

CAccountEncryptToolView::CAccountEncryptToolView()
{
	// TODO: �ڴ˴���ӹ������

}

CAccountEncryptToolView::~CAccountEncryptToolView()
{
}

BOOL CAccountEncryptToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CListView::PreCreateWindow(cs);
}

void CAccountEncryptToolView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: ���� GetListCtrl() ֱ�ӷ��� ListView ���б�ؼ���
	//  �Ӷ������������ ListView��
	CListCtrl& ListCtrl = GetListCtrl();
	CHeaderCtrl* pHeader = ListCtrl.GetHeaderCtrl();
	if (pHeader == NULL)
	{
		SetWindowTheme(m_hWnd,L"Explorer",NULL);
		ModifyStyle(LVS_TYPEMASK , LVS_REPORT , 0);
		ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);
		int nIndex=0;
		ListCtrl.InsertColumn(nIndex++,_T("���"));
		ListCtrl.InsertColumn(nIndex++,_T("����"));
		ListCtrl.InsertColumn(nIndex++,_T("��չ��Ϣ"));
		ListCtrl.EnableGroupView(TRUE);
	}
	AdjustListWidth();
}

// CAccountEncryptToolView ���

#ifdef _DEBUG
void CAccountEncryptToolView::AssertValid() const
{
	CListView::AssertValid();
}

void CAccountEncryptToolView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CAccountEncryptToolDoc* CAccountEncryptToolView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAccountEncryptToolDoc)));
	return (CAccountEncryptToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CAccountEncryptToolView ��Ϣ�������
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