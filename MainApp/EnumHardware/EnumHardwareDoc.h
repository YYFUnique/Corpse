
// EnumHardwareDoc.h : CEnumHardwareDoc ��Ľӿ�
//


#pragma once


class CEnumHardwareDoc : public CDocument
{
protected: // �������л�����
	CEnumHardwareDoc();
	DECLARE_DYNCREATE(CEnumHardwareDoc)

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
	virtual ~CEnumHardwareDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


