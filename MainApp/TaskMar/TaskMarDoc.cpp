
// TaskMarDoc.cpp : CTaskMarDoc 类的实现
//

#include "stdafx.h"
#include "TaskMar.h"

#include "TaskMarDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTaskMarDoc

IMPLEMENT_DYNCREATE(CTaskMarDoc, CDocument)

BEGIN_MESSAGE_MAP(CTaskMarDoc, CDocument)
END_MESSAGE_MAP()


// CTaskMarDoc 构造/析构

CTaskMarDoc::CTaskMarDoc()
{
	// TODO: 在此添加一次性构造代码

}

CTaskMarDoc::~CTaskMarDoc()
{
}

BOOL CTaskMarDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

BOOL CTaskMarDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (CDocument::OnSaveDocument(lpszPathName) == FALSE)
		return FALSE;

	return TRUE;
}



// CTaskMarDoc 序列化

void CTaskMarDoc::Serialize(CArchive& ar)
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


// CTaskMarDoc 诊断

#ifdef _DEBUG
void CTaskMarDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTaskMarDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTaskMarDoc 命令
CView * CTaskMarDoc::GetView(CRuntimeClass* pClass)
{
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(pClass))
			return pView;
	}
	return NULL;
}