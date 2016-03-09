
// PEToolDoc.cpp : CPEToolDoc 类的实现
//

#include "stdafx.h"
#include "PETool.h"

#include "PEToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPEToolDoc

IMPLEMENT_DYNCREATE(CPEToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CPEToolDoc, CDocument)
END_MESSAGE_MAP()


// CPEToolDoc 构造/析构

CPEToolDoc::CPEToolDoc()
{
	// TODO: 在此添加一次性构造代码

}

CPEToolDoc::~CPEToolDoc()
{
}

BOOL CPEToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	SetTitle(_T("飞逸无限PE分析工具"));

	return TRUE;
}




// CPEToolDoc 序列化

void CPEToolDoc::Serialize(CArchive& ar)
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


// CPEToolDoc 诊断

#ifdef _DEBUG
void CPEToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPEToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPEToolDoc 命令
CView* CPEToolDoc::GetView(CRuntimeClass* pClass)
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