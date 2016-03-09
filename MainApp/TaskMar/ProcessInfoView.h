#ifndef _PROCESSINFO_VIEW_H
#define _PROCESSINFO_VIEW_H

#include "ProcessInfoDlg/TabCtrlProcessInfo.h"
#include "Resource.h"
#define  PROCESS_TABCTRL_NUM				6//显示进程信息的tab控件的Item数量

class CProcessInfoView : public CFormView
{
protected: // create from serialization only
	CProcessInfoView();
	DECLARE_DYNCREATE(CProcessInfoView)

public:
	//{{AFX_DATA(CTabbedToolbarView)
	enum{ IDD = IDD_TABBEDTOOLBAR_FORM };
	//}}AFX_DATA

	// Attributes
public:
	CTaskMarDoc* GetDocument();

	CTabCtrl m_wndTabControl;
	CTabCtrlProcessInfo* m_TabCtrlInfo[PROCESS_TABCTRL_NUM];

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL UpdateProcessInfo();
	BOOL SelectTabCtrl(UINT nItem);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CProcessInfoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CImageList m_TabCtrlImgList;
	// Generated message map functions
protected:
	//{{AFX_MSG(CTabbedToolbarView)
	//}}AFX_MSG
	afx_msg void OnTabSelectChange(NMHDR* pNMHDR,LRESULT* lResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TabbedToolbarView.cpp
inline CTaskMarDoc* CProcessInfoView::GetDocument()
{ return (CTaskMarDoc*)m_pDocument; }
#endif

#endif //_PROCESSINFO_VIEW_H