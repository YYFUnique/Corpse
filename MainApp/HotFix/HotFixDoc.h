
// HotFixDoc.h : CHotFixDoc ��Ľӿ�
//


#pragma once


class CHotFixDoc : public CDocument
{
protected: // �������л�����
	CHotFixDoc();
	DECLARE_DYNCREATE(CHotFixDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CHotFixDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


