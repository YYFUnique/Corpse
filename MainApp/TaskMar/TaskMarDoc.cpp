
// TaskMarDoc.cpp : CTaskMarDoc ���ʵ��
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


// CTaskMarDoc ����/����

CTaskMarDoc::CTaskMarDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CTaskMarDoc::~CTaskMarDoc()
{
}

BOOL CTaskMarDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}

BOOL CTaskMarDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (CDocument::OnSaveDocument(lpszPathName) == FALSE)
		return FALSE;

	return TRUE;
}



// CTaskMarDoc ���л�

void CTaskMarDoc::Serialize(CArchive& ar)
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


// CTaskMarDoc ���

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


// CTaskMarDoc ����
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