#pragma once
#include "NetTransport/WinSocket.h"
#include "ClientAppDoc.h"
#include "Include/Data.h"

typedef struct _LIST_PARAM_
{
	DWORD dwConnID;
	CONNECT_STAT ConStat;
}LIST_PARAM,*PLIST_PARAM;

typedef CList<LIST_PARAM,LIST_PARAM&> CLIST_PARAM;

class CClientAppView : public CListView
{
protected: // �������л�����
	CClientAppView();
	DECLARE_DYNCREATE(CClientAppView)

// ����
public:
	CClientAppDoc* GetDocument() const;

// ����
public:
	void AdjustColumnWidth();
	BOOL InsertIntoList(DWORD dwConnID,LPCTSTR lpszAddrAndPort,LPCTSTR lpszUserInfo);
	BOOL DeleteItem(DWORD dwConnID);
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���
	
// ʵ��
public:
	virtual ~CClientAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int m_nLastSortSubItem;	//��һ�ν���������
	CLIST_PARAM m_ListParam;	//���������ӵ�״̬
	int GetCheckLine(UINT nFlags, CPoint point);
// ���ɵ���Ϣӳ�亯��
protected:
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRClick(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnListViewCmd(UINT nID);
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg BOOL OnColumnClk(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ClientAppView.cpp �еĵ��԰汾
inline CClientAppDoc* CClientAppView::GetDocument() const
   { return reinterpret_cast<CClientAppDoc*>(m_pDocument); }
#endif

