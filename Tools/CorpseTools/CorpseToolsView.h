#pragma once
#include "Group/ToolItemBase.h"
typedef CList<CToolItemBase*,CToolItemBase*> CToolItemList;

class CCorpseToolsView : public CListView
{
protected: // �������л�����
	CCorpseToolsView();
	DECLARE_DYNCREATE(CCorpseToolsView)

// ����
public:
	CCorpseToolsDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CCorpseToolsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CToolItemList  m_pToolItemList;
	CImageList		m_IconImageList;
private:
	/************************************************************************/
	// �������ƣ�ClearToolItem
	// �������ܣ������ͼ�д�����Item
	// ������������
	// ��������ֵ�����ִ�гɹ�����TRUE�����򷵻�FALSE
	/************************************************************************/
	BOOL ClearToolItem();
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // CorpseToolsView.cpp �еĵ��԰汾
inline CCorpseToolsDoc* CCorpseToolsView::GetDocument() const
   { return reinterpret_cast<CCorpseToolsDoc*>(m_pDocument); }
#endif

