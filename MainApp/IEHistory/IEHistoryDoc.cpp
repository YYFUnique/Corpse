
// IEHistoryDoc.cpp : CIEHistoryDoc 类的实现
//

#include "stdafx.h"
#include "IEHistory.h"

#include "IEHistoryDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIEHistoryDoc

IMPLEMENT_DYNCREATE(CIEHistoryDoc, CDocument)

BEGIN_MESSAGE_MAP(CIEHistoryDoc, CDocument)
END_MESSAGE_MAP()


// CIEHistoryDoc 构造/析构

CIEHistoryDoc::CIEHistoryDoc()
{
	// TODO: 在此添加一次性构造代码

}

CIEHistoryDoc::~CIEHistoryDoc()
{
}

BOOL CIEHistoryDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CIEHistoryDoc 序列化

void CIEHistoryDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CIEHistoryDoc 诊断

#ifdef _DEBUG
void CIEHistoryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIEHistoryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CIEHistoryDoc 命令
CView* CIEHistoryDoc::GetView(CRuntimeClass* pClass)
{
	POSITION pos = GetFirstViewPosition();
	CView* pView = NULL;
	while(pos)
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(pClass))
			break;
	}

	return pView;
}