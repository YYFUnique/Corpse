
// EnumHardwareDoc.cpp : CEnumHardwareDoc 类的实现
//

#include "stdafx.h"
#include "EnumHardware.h"

#include "EnumHardwareDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEnumHardwareDoc

IMPLEMENT_DYNCREATE(CEnumHardwareDoc, CDocument)

BEGIN_MESSAGE_MAP(CEnumHardwareDoc, CDocument)
END_MESSAGE_MAP()


// CEnumHardwareDoc 构造/析构

CEnumHardwareDoc::CEnumHardwareDoc()
{
	// TODO: 在此添加一次性构造代码

}

CEnumHardwareDoc::~CEnumHardwareDoc()
{
}

BOOL CEnumHardwareDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CEnumHardwareDoc 序列化

void CEnumHardwareDoc::Serialize(CArchive& ar)
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


// CEnumHardwareDoc 诊断

#ifdef _DEBUG
void CEnumHardwareDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEnumHardwareDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEnumHardwareDoc 命令
