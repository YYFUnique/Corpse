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
protected: // 仅从序列化创建
	CClientAppView();
	DECLARE_DYNCREATE(CClientAppView)

// 属性
public:
	CClientAppDoc* GetDocument() const;

// 操作
public:
	void AdjustColumnWidth();
	BOOL InsertIntoList(DWORD dwConnID,LPCTSTR lpszAddrAndPort,LPCTSTR lpszUserInfo);
	BOOL DeleteItem(DWORD dwConnID);
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	
// 实现
public:
	virtual ~CClientAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int m_nLastSortSubItem;	//上一次进行排序列
	CLIST_PARAM m_ListParam;	//保存新连接的状态
	int GetCheckLine(UINT nFlags, CPoint point);
// 生成的消息映射函数
protected:
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRClick(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg void OnListViewCmd(UINT nID);
	afx_msg void OnNMCustomdraw(NMHDR* pNMHDR,LRESULT* pResult);
	afx_msg BOOL OnColumnClk(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ClientAppView.cpp 中的调试版本
inline CClientAppDoc* CClientAppView::GetDocument() const
   { return reinterpret_cast<CClientAppDoc*>(m_pDocument); }
#endif

