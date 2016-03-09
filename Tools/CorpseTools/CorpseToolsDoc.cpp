
// CorpseToolsDoc.cpp : CCorpseToolsDoc 类的实现
//

#include "stdafx.h"
#include "CorpseTools.h"

#include "CorpseToolsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCorpseToolsDoc

IMPLEMENT_DYNCREATE(CCorpseToolsDoc, CDocument)

BEGIN_MESSAGE_MAP(CCorpseToolsDoc, CDocument)
END_MESSAGE_MAP()


// CCorpseToolsDoc 构造/析构

CCorpseToolsDoc::CCorpseToolsDoc()
{
	// TODO: 在此添加一次性构造代码

}

CCorpseToolsDoc::~CCorpseToolsDoc()
{
}

BOOL CCorpseToolsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CCorpseToolsDoc 序列化

void CCorpseToolsDoc::Serialize(CArchive& ar)
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


// CCorpseToolsDoc 诊断

#ifdef _DEBUG
void CCorpseToolsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCorpseToolsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCorpseToolsDoc 命令
