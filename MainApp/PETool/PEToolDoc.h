
// PEToolDoc.h : CPEToolDoc ��Ľӿ�
//


#pragma once


class CPEToolDoc : public CDocument
{
protected: // �������л�����
	CPEToolDoc();
	DECLARE_DYNCREATE(CPEToolDoc)

// ����
public:

// ����
public:
	CView* GetView(CRuntimeClass* pClass);
// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CPEToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


