
// IEHistoryDoc.h : CIEHistoryDoc ��Ľӿ�
//


#pragma once


class CIEHistoryDoc : public CDocument
{
protected: // �������л�����
	CIEHistoryDoc();
	DECLARE_DYNCREATE(CIEHistoryDoc)

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
	virtual ~CIEHistoryDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


