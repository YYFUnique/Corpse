
// ClientAppDoc.cpp : CClientAppDoc ���ʵ��
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


// CClientAppDoc ����/����

CClientAppDoc::CClientAppDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CClientAppDoc::~CClientAppDoc()
{
}

BOOL CClientAppDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CClientAppDoc ���л�

void CClientAppDoc::Serialize(CArchive& ar)
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


// CClientAppDoc ���

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


// CClientAppDoc ����
