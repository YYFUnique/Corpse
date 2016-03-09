#pragma once
#include "Group/ToolItemBase.h"
typedef CList<CToolItemBase*,CToolItemBase*> CToolItemList;

class CCorpseToolsView : public CListView
{
protected: // 仅从序列化创建
	CCorpseToolsView();
	DECLARE_DYNCREATE(CCorpseToolsView)

// 属性
public:
	CCorpseToolsDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
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
	// 函数名称：ClearToolItem
	// 函数功能：清除视图中创建的Item
	// 函数参数：无
	// 函数返回值：如果执行成功返回TRUE，否则返回FALSE
	/************************************************************************/
	BOOL ClearToolItem();
// 生成的消息映射函数
protected:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // CorpseToolsView.cpp 中的调试版本
inline CCorpseToolsDoc* CCorpseToolsView::GetDocument() const
   { return reinterpret_cast<CCorpseToolsDoc*>(m_pDocument); }
#endif

