
// TaskMarDoc.h : CTaskMarDoc ��Ľӿ�
//


#pragma once


class CTaskMarDoc : public CDocument
{
protected: // �������л�����
	CTaskMarDoc();
	DECLARE_DYNCREATE(CTaskMarDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CTaskMarDoc();
	CView* GetView(CRuntimeClass* pClass);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


