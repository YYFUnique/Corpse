// IEHistoryDetail.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IEHistory.h"
#include "IEHistoryDetail.h"


// CIEHistoryDetail

IMPLEMENT_DYNCREATE(CIEHistoryDetail, CHtmlView)

CIEHistoryDetail::CIEHistoryDetail()
{

}

CIEHistoryDetail::~CIEHistoryDetail()
{
}

void CIEHistoryDetail::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIEHistoryDetail, CHtmlView)
END_MESSAGE_MAP()


// CIEHistoryDetail ���

#ifdef _DEBUG
void CIEHistoryDetail::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CIEHistoryDetail::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


// CIEHistoryDetail ��Ϣ�������
void CIEHistoryDetail::SetContent(LPCTSTR lpszDescribe)
{
	//CHtmlEditCtrl& HtmlCtrl = get();
}