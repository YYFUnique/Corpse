
// HotFixDoc.cpp : CHotFixDoc ���ʵ��
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


// CHotFixDoc ����/����

CHotFixDoc::CHotFixDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CHotFixDoc::~CHotFixDoc()
{
}

BOOL CHotFixDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CHotFixDoc ���л�

void CHotFixDoc::Serialize(CArchive& ar)
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


// CHotFixDoc ���

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


// CHotFixDoc ����
