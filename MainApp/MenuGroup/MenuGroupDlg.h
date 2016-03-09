
// MenuGroupDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
typedef struct _tagTREEITEMINFO
{
	CString strIconPath;
	CString strRunCmd;
	CString strName;		//这个是要保存在注册表中的名称，不是Tree上显示的名称
}TREEITEMINFO;
typedef CList<TREEITEMINFO,TREEITEMINFO&> CMenuRunCmd;
// CMenuGroupDlg 对话框
class CMenuGroupDlg : public CDialog
{
// 构造
public:
	CMenuGroupDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MENUGROUP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

protected:
	UINT GetFileIcon(LPCTSTR lpszFilePath);
	HTREEITEM CopyItem(HTREEITEM hItem,HTREEITEM hTreeParent,HTREEITEM hTreeAfter);
	void RemoveItemInfo(HTREEITEM hTreeItem);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnLoadMenuGroup(WPARAM wParam,LPARAM lParam);
	afx_msg void OnRclickList(NMHDR* pNMHdr,LPARAM* lResult);
	afx_msg void OnDbClick(NMHDR* pNMHdr,LPARAM* lResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddItem();
	afx_msg void OnEditItem();
	afx_msg void OnDelItem();
	afx_msg void OnItemUp();
	afx_msg void OnItemDown();
	DECLARE_MESSAGE_MAP()
	CTreeCtrl m_TreeCtrl;
	CImageList m_ImageList;
	CMenuRunCmd m_MenuRunCmd;
public:
	afx_msg void OnBnClickedBtnCreate();
	afx_msg void OnBnClickedBtnRemove();
};
