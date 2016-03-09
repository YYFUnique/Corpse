
// LeftView.h : CLeftView 类的接口
//


#pragma once

class CPEToolDoc;
class CPEToolView;
class CPELibrary;
class CLeftView : public CTreeView
{
protected: // 仅从序列化创建
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// 属性
public:
	CPEToolDoc* GetDocument();

// 操作
public:
	BOOL InsertPEHeaderInfo(CPELibrary* pPELibrary);
// 重写
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CPEToolView* m_pPEToolView;
	CPELibrary* m_pPELibrary;
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnItemChanged(NMHDR* pNMHdr,LPARAM* lResult);
};

#ifndef _DEBUG  // LeftView.cpp 中的调试版本
inline CPEToolDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CPEToolDoc*>(m_pDocument); }
#endif

