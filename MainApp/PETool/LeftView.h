
// LeftView.h : CLeftView ��Ľӿ�
//


#pragma once

class CPEToolDoc;
class CPEToolView;
class CPELibrary;
class CLeftView : public CTreeView
{
protected: // �������л�����
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// ����
public:
	CPEToolDoc* GetDocument();

// ����
public:
	BOOL InsertPEHeaderInfo(CPELibrary* pPELibrary);
// ��д
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CPEToolView* m_pPEToolView;
	CPELibrary* m_pPELibrary;
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnItemChanged(NMHDR* pNMHdr,LPARAM* lResult);
};

#ifndef _DEBUG  // LeftView.cpp �еĵ��԰汾
inline CPEToolDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CPEToolDoc*>(m_pDocument); }
#endif

