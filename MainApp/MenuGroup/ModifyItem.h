#pragma once
#include "MenuGroupDlg.h"

// CModifyItem 对话框
typedef CMap<DWORD,DWORD,TREEITEMINFO,TREEITEMINFO&> CTreeItemMap;

class CModifyItem : public CDialog
{
	DECLARE_DYNAMIC(CModifyItem)

public:
	CModifyItem(LPCTSTR lpszItemName,LPCTSTR lpszItemIcon = NULL,LPCTSTR lpszRunCmd = NULL,LPCTSTR lpszRegName = NULL,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModifyItem();
	CString& GetItemName();
	BOOL GetItemInfo(TREEITEMINFO& TreeItemInfo);
// 对话框数据
	enum { IDD = IDD_DIALOG_ADD_ITEM };
protected:
	CString m_strItemName;
	CString m_strIconPath;
	CString m_strRunCmd;
	CString m_strRegName;
	BOOL	m_bOpen;
	CTreeItemMap	m_TreeItemInfo;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLookup();
	afx_msg void OnBnClickedBtnPrevSet();
	afx_msg void OnRadioClick(UINT nID);
	afx_msg LRESULT OnLoadItemInfo(WPARAM wParam,LPARAM lParam);
};
