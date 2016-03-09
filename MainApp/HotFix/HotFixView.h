
// HotFixView.h : CHotFixView ��Ľӿ�
//


#pragma once


class CHotFixView : public CListView
{
protected: // �������л�����
	CHotFixView();
	DECLARE_DYNCREATE(CHotFixView)

// ����
public:
	CHotFixDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CHotFixView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnShowHotFix(WPARAM wParam,LPARAM lParam);
};

#ifndef _DEBUG  // HotFixView.cpp �еĵ��԰汾
inline CHotFixDoc* CHotFixView::GetDocument() const
   { return reinterpret_cast<CHotFixDoc*>(m_pDocument); }
#endif

