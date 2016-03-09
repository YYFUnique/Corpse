
// IEHistoryView.h : CIEHistoryView ��Ľӿ�
//


#pragma once
#include "IEHistoryModule.h"
#include "Class/CheckItem.h"

class CIEHistoryView : public CListView
{
protected: // �������л�����
	CIEHistoryView();
	DECLARE_DYNCREATE(CIEHistoryView)

// ����
public:
	CIEHistoryDoc* GetDocument() const;

// ����
public:
	void SetListContent(MODULE_TYPE ModuleType);
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CIEHistoryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CCheckItem* m_pCheckItem;
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // IEHistoryView.cpp �еĵ��԰汾
inline CIEHistoryDoc* CIEHistoryView::GetDocument() const
   { return reinterpret_cast<CIEHistoryDoc*>(m_pDocument); }
#endif

