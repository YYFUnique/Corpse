
// IEHistoryDoc.cpp : CIEHistoryDoc ���ʵ��
//

#include "stdafx.h"
#include "IEHistory.h"

#include "IEHistoryDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIEHistoryDoc

IMPLEMENT_DYNCREATE(CIEHistoryDoc, CDocument)

BEGIN_MESSAGE_MAP(CIEHistoryDoc, CDocument)
END_MESSAGE_MAP()


// CIEHistoryDoc ����/����

CIEHistoryDoc::CIEHistoryDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CIEHistoryDoc::~CIEHistoryDoc()
{
}

BOOL CIEHistoryDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CIEHistoryDoc ���л�

void CIEHistoryDoc::Serialize(CArchive& ar)
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


// CIEHistoryDoc ���

#ifdef _DEBUG
void CIEHistoryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIEHistoryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CIEHistoryDoc ����
CView* CIEHistoryDoc::GetView(CRuntimeClass* pClass)
{
	POSITION pos = GetFirstViewPosition();
	CView* pView = NULL;
	while(pos)
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(pClass))
			break;
	}

	return pView;
}