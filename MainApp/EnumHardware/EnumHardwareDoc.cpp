
// EnumHardwareDoc.cpp : CEnumHardwareDoc ���ʵ��
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


// CEnumHardwareDoc ����/����

CEnumHardwareDoc::CEnumHardwareDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CEnumHardwareDoc::~CEnumHardwareDoc()
{
}

BOOL CEnumHardwareDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CEnumHardwareDoc ���л�

void CEnumHardwareDoc::Serialize(CArchive& ar)
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


// CEnumHardwareDoc ���

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


// CEnumHardwareDoc ����
