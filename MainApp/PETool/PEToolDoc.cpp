
// PEToolDoc.cpp : CPEToolDoc ���ʵ��
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


// CPEToolDoc ����/����

CPEToolDoc::CPEToolDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CPEToolDoc::~CPEToolDoc()
{
}

BOOL CPEToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)
	SetTitle(_T("��������PE��������"));

	return TRUE;
}




// CPEToolDoc ���л�

void CPEToolDoc::Serialize(CArchive& ar)
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


// CPEToolDoc ���

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


// CPEToolDoc ����
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