#pragma once

#include "ProcessInfo.h"
// CTabCtrlProcessInfo �Ի���
#include "../Resource.h"
class CTabCtrlProcessInfo : public CDialog
{
	DECLARE_DYNAMIC(CTabCtrlProcessInfo)

public:
	CTabCtrlProcessInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTabCtrlProcessInfo();
	BOOL Create(CWnd* pParent,DWORD dwListId);
// �Ի�������
	enum { IDD = IDD_DIALOG_LIST_INFO };
	BOOL ShowTabCtrlInfo();
	UINT GetTabIcon();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnRClick(UINT nID,NMHDR* pNMHdr,LPARAM* lResult);
	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_ListCtrl;
	DWORD m_ListId;
	CProcessInfo* m_pProcessInfo;
public:
	afx_msg void OnLookupThreadFile();
};
