
// EnumHardwareView.h : CEnumHardwareView 类的接口
//


#pragma once
#include "LsCommon/Class/Hardware.h"
typedef struct tgaDevName 
{
	CString strDevName;
	GUID DevGuid;
	UINT  DevIcon;
}DEVNAME,*PDEVNAME;

typedef CList<DEVNAME,DEVNAME&>CDevNameList;

class CEnumHardwareView : public CTreeView
{
protected: // 仅从序列化创建
	CEnumHardwareView();
	DECLARE_DYNCREATE(CEnumHardwareView)

// 属性
public:
	CEnumHardwareDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CEnumHardwareView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bShowAll;
	CDevInfoList m_DevInfoList;
	CDevNameList m_DevNameList;
	CImageList m_ImgList;
// 生成的消息映射函数
protected:
	afx_msg LRESULT OnShowHardware(WPARAM wParam,LPARAM lParam);
	afx_msg void OnRClick(NMHDR* pNMHdr,LPARAM* lResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowall();
	afx_msg void OnDonotShowHide();
	afx_msg void OnHardwareScanf();
	afx_msg void OnHardwareProperty();
};

#ifndef _DEBUG  // EnumHardwareView.cpp 中的调试版本
inline CEnumHardwareDoc* CEnumHardwareView::GetDocument() const
   { return reinterpret_cast<CEnumHardwareDoc*>(m_pDocument); }
#endif

