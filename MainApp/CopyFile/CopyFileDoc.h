
// CopyFileDoc.h : CCopyFileDoc ��Ľӿ�
//


#pragma once


class CCopyFileDoc : public CDocument
{
protected: // �������л�����
	CCopyFileDoc();
	DECLARE_DYNCREATE(CCopyFileDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

	virtual void Serialize(CArchive& ar);
	//virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
// ʵ��
public:
	virtual ~CCopyFileDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAddItemToList();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
public:

};


