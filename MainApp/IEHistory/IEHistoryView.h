
// IEHistoryView.h : CIEHistoryView 类的接口
//


#pragma once
#include "IEHistoryModule.h"
#include "Class/CheckItem.h"

class CIEHistoryView : public CListView
{
protected: // 仅从序列化创建
	CIEHistoryView();
	DECLARE_DYNCREATE(CIEHistoryView)

// 属性
public:
	CIEHistoryDoc* GetDocument() const;

// 操作
public:
	void SetListContent(MODULE_TYPE ModuleType);
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CIEHistoryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CCheckItem* m_pCheckItem;
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // IEHistoryView.cpp 中的调试版本
inline CIEHistoryDoc* CIEHistoryView::GetDocument() const
   { return reinterpret_cast<CIEHistoryDoc*>(m_pDocument); }
#endif

