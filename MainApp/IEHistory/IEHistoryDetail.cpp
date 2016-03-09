// IEHistoryDetail.cpp : 实现文件
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


// CIEHistoryDetail 诊断

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


// CIEHistoryDetail 消息处理程序
void CIEHistoryDetail::SetContent(LPCTSTR lpszDescribe)
{
	//CHtmlEditCtrl& HtmlCtrl = get();
}