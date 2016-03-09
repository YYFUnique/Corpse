
// HotFixView.h : CHotFixView 类的接口
//


#pragma once


class CHotFixView : public CListView
{
protected: // 仅从序列化创建
	CHotFixView();
	DECLARE_DYNCREATE(CHotFixView)

// 属性
public:
	CHotFixDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CHotFixView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnShowHotFix(WPARAM wParam,LPARAM lParam);
};

#ifndef _DEBUG  // HotFixView.cpp 中的调试版本
inline CHotFixDoc* CHotFixView::GetDocument() const
   { return reinterpret_cast<CHotFixDoc*>(m_pDocument); }
#endif

