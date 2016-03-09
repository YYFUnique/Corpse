
// IEHistoryView.cpp : CIEHistoryView ���ʵ��
//

#include "stdafx.h"
#include "IEHistory.h"

#include "IEHistoryDoc.h"
#include "IEHistoryView.h"

#include "LsCommon/CommonFunc.h"
#include "IEHistoryDetail.h"
#include "LsCommon/Browser/Browser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIEHistoryView

IMPLEMENT_DYNCREATE(CIEHistoryView, CListView)

BEGIN_MESSAGE_MAP(CIEHistoryView, CListView)
END_MESSAGE_MAP()

// CIEHistoryView ����/����

CIEHistoryView::CIEHistoryView()
{
	// TODO: �ڴ˴���ӹ������
	m_pCheckItem = NULL;
}

CIEHistoryView::~CIEHistoryView()
{
	
}

BOOL CIEHistoryView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CListView::PreCreateWindow(cs);
}

void CIEHistoryView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: ���� GetListCtrl() ֱ�ӷ��� ListView ���б�ؼ���
	//  �Ӷ������������ ListView��
	CListCtrl& ListCtrl = GetListCtrl();
	SetWindowTheme(m_hWnd,L"Explorer",NULL);
	ModifyStyle(LVS_TYPEMASK , LVS_REPORT , 0);

	ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_SINGLEROW|LVS_EX_HEADERDRAGDROP|LVS_EX_DOUBLEBUFFER);

	AdjustListWidth(ListCtrl);
}


// CIEHistoryView ���

#ifdef _DEBUG
void CIEHistoryView::AssertValid() const
{
	CListView::AssertValid();
}

void CIEHistoryView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CIEHistoryDoc* CIEHistoryView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIEHistoryDoc)));
	return (CIEHistoryDoc*)m_pDocument;
}
#endif //_DEBUG


// CIEHistoryView ��Ϣ�������

void CIEHistoryView::SetListContent(MODULE_TYPE ModuleType)
{
	if (m_pCheckItem != NULL)
	{
		m_pCheckItem->Release();
		m_pCheckItem = NULL;
	}

	CListCtrl& ListCtrl = GetListCtrl();
	ListCtrl.SetRedraw(FALSE);

	int nTotl = ListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i=nTotl;i>=0;i--)
	{
		ListCtrl.GetHeaderCtrl()->DeleteItem(i);
	}
	
	ListCtrl.DeleteAllItems();

	CIEHistoryDoc* pDoc = (CIEHistoryDoc*)GetDocument();
	CIEHistoryDetail* pView = (CIEHistoryDetail*)(pDoc->GetView(RUNTIME_CLASS(CIEHistoryDetail)));

	m_pCheckItem = CCheckItem::CreateCheckItem(ModuleType);
	ASSERT(m_pCheckItem);

	CString strDescribe;
	m_pCheckItem->GetItemDescribe(strDescribe);
	pView->SetContent(strDescribe);
	

	CStringArray strListHeader;
	
	m_pCheckItem->GetListCtrlHeader(strListHeader);
	for (int i=0;i<strListHeader.GetSize();++i)
		ListCtrl.InsertColumn(i,strListHeader.GetAt(i));

 	CItemInfoList ItemInfoList;

	CBrowser* pBrowser = CBrowser::CreateBrowser(BROWSRETYPE_IE);
	ASSERT(pBrowser);
	m_pCheckItem->Lanuch(ItemInfoList);

	if (pBrowser != NULL)
	{
		pBrowser->Release();
		pBrowser = NULL;
	}

	if (m_pCheckItem != NULL)
	{
		m_pCheckItem->Release();
		m_pCheckItem = NULL;
	}

	POSITION pos = ItemInfoList.GetHeadPosition();
	CString strVisitedTime;
	while(pos)
	{
		ITEMINFO& ItemInfo = ItemInfoList.GetNext(pos);
		int nItem = ListCtrl.InsertItem(0x7FFFFFFF,ItemInfo.strTitle);
		ListCtrl.SetItemText(nItem,1,ItemInfo.strAddr);
		
		CTime LastVisitedTime(ItemInfo.dwVisitedTime);
		strVisitedTime.Format(_T("%d-%02d-%02d %02d:%02d:%02d"),
							LastVisitedTime.GetYear(),LastVisitedTime.GetMonth(),LastVisitedTime.GetDay(),
							LastVisitedTime.GetHour(),LastVisitedTime.GetMinute(),LastVisitedTime.GetSecond());

		ListCtrl.SetItemText(nItem,2,strVisitedTime);
	}

	AdjustListWidth(ListCtrl);
	ListCtrl.SetRedraw(TRUE);
}