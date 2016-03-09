#pragma once
#include "MenuGroupDlg.h"

// CModifyItem �Ի���
typedef CMap<DWORD,DWORD,TREEITEMINFO,TREEITEMINFO&> CTreeItemMap;

class CModifyItem : public CDialog
{
	DECLARE_DYNAMIC(CModifyItem)

public:
	CModifyItem(LPCTSTR lpszItemName,LPCTSTR lpszItemIcon = NULL,LPCTSTR lpszRunCmd = NULL,LPCTSTR lpszRegName = NULL,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CModifyItem();
	CString& GetItemName();
	BOOL GetItemInfo(TREEITEMINFO& TreeItemInfo);
// �Ի�������
	enum { IDD = IDD_DIALOG_ADD_ITEM };
protected:
	CString m_strItemName;
	CString m_strIconPath;
	CString m_strRunCmd;
	CString m_strRegName;
	BOOL	m_bOpen;
	CTreeItemMap	m_TreeItemInfo;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLookup();
	afx_msg void OnBnClickedBtnPrevSet();
	afx_msg void OnRadioClick(UINT nID);
	afx_msg LRESULT OnLoadItemInfo(WPARAM wParam,LPARAM lParam);
};
