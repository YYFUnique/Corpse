
// HotFixDoc.cpp : CHotFixDoc 类的实现
//

#include "stdafx.h"
#include "HotFix.h"

#include "HotFixDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHotFixDoc

IMPLEMENT_DYNCREATE(CHotFixDoc, CDocument)

BEGIN_MESSAGE_MAP(CHotFixDoc, CDocument)
END_MESSAGE_MAP()


// CHotFixDoc 构造/析构

CHotFixDoc::CHotFixDoc()
{
	// TODO: 在此添加一次性构造代码

}

CHotFixDoc::~CHotFixDoc()
{
}

BOOL CHotFixDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CHotFixDoc 序列化

void CHotFixDoc::Serialize(CArchive& ar)
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


// CHotFixDoc 诊断

#ifdef _DEBUG
void CHotFixDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHotFixDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CHotFixDoc 命令
