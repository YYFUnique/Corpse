
// CorpseToolsDoc.cpp : CCorpseToolsDoc ���ʵ��
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


// CCorpseToolsDoc ����/����

CCorpseToolsDoc::CCorpseToolsDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CCorpseToolsDoc::~CCorpseToolsDoc()
{
}

BOOL CCorpseToolsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CCorpseToolsDoc ���л�

void CCorpseToolsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CCorpseToolsDoc ���

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


// CCorpseToolsDoc ����
