
// EnumHardwareView.h : CEnumHardwareView ��Ľӿ�
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
protected: // �������л�����
	CEnumHardwareView();
	DECLARE_DYNCREATE(CEnumHardwareView)

// ����
public:
	CEnumHardwareDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
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
// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // EnumHardwareView.cpp �еĵ��԰汾
inline CEnumHardwareDoc* CEnumHardwareView::GetDocument() const
   { return reinterpret_cast<CEnumHardwareDoc*>(m_pDocument); }
#endif

