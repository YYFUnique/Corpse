
// MenuGroupDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
typedef struct _tagTREEITEMINFO
{
	CString strIconPath;
	CString strRunCmd;
	CString strName;		//�����Ҫ������ע����е����ƣ�����Tree����ʾ������
}TREEITEMINFO;
typedef CList<TREEITEMINFO,TREEITEMINFO&> CMenuRunCmd;
// CMenuGroupDlg �Ի���
class CMenuGroupDlg : public CDialog
{
// ����
public:
	CMenuGroupDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MENUGROUP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

protected:
	UINT GetFileIcon(LPCTSTR lpszFilePath);
	HTREEITEM CopyItem(HTREEITEM hItem,HTREEITEM hTreeParent,HTREEITEM hTreeAfter);
	void RemoveItemInfo(HTREEITEM hTreeItem);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
