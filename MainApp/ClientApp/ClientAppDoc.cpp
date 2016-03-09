
// ClientAppDoc.cpp : CClientAppDoc 类的实现
//

#include "stdafx.h"
#include "ClientApp.h"

#include "ClientAppDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientAppDoc

IMPLEMENT_DYNCREATE(CClientAppDoc, CDocument)

BEGIN_MESSAGE_MAP(CClientAppDoc, CDocument)
END_MESSAGE_MAP()


// CClientAppDoc 构造/析构

CClientAppDoc::CClientAppDoc()
{
	// TODO: 在此添加一次性构造代码

}

CClientAppDoc::~CClientAppDoc()
{
}

BOOL CClientAppDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CClientAppDoc 序列化

void CClientAppDoc::Serialize(CArchive& ar)
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


// CClientAppDoc 诊断

#ifdef _DEBUG
void CClientAppDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClientAppDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CClientAppDoc 命令
