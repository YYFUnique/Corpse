
// AccountEncryptToolView.h : CAccountEncryptToolView 类的接口
//


#pragma once


class CAccountEncryptToolView : public CListView
{
protected: // 仅从序列化创建
	CAccountEncryptToolView();
	DECLARE_DYNCREATE(CAccountEncryptToolView)

// 属性
public:
	CAccountEncryptToolDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CAccountEncryptToolView();
	BOOL AddGroup(LPCTSTR lpszGroupName,int nGroupId);
	BOOL AddItemToGroup(ACCOUNT_INFO& AccountInfo);
	BOOL AdjustListWidth();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // AccountEncryptToolView.cpp 中的调试版本
inline CAccountEncryptToolDoc* CAccountEncryptToolView::GetDocument() const
   { return reinterpret_cast<CAccountEncryptToolDoc*>(m_pDocument); }
#endif

