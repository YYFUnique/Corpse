
// CorpseToolsDoc.h : CCorpseToolsDoc ��Ľӿ�
//


#pragma once


class CCorpseToolsDoc : public CDocument
{
protected: // �������л�����
	CCorpseToolsDoc();
	DECLARE_DYNCREATE(CCorpseToolsDoc)

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
	virtual ~CCorpseToolsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


