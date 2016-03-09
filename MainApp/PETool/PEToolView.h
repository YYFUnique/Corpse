
// PEToolView.h : CPEToolView 类的接口
//


#pragma once
class CPELibrary;

typedef enum tagITEM_DATA_INFO
{
	ITEM_DATA_INFO_DOS_HEADER = 0,			//PE文件DOS头
	ITEM_DATA_INFO_FILE_HEADER = 0,			//PE文件DOS头
	ITEM_DATA_INFO_OPTION_HEADER = 0,			//PE文件DOS头
}ITEM_DATA_INFO;

class CPEToolView : public CListView
{
protected: // 仅从序列化创建
	CPEToolView();
	DECLARE_DYNCREATE(CPEToolView)

// 属性
public:
	CPEToolDoc* GetDocument() const;
	void OnShowItemData(ITEM_DATA_INFO ItemDataInfo,CPELibrary* pPELibrary);

	void ShowItemDosHeader(CPELibrary* pPELibrary);
	void ShowItemFileHeader(CPELibrary* pPELibrary);
// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CPEToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // PEToolView.cpp 中的调试版本
inline CPEToolDoc* CPEToolView::GetDocument() const
   { return reinterpret_cast<CPEToolDoc*>(m_pDocument); }
#endif

