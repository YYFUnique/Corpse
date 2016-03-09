
// ConvertToolsDlg.h : 头文件
//

#pragma once
#define TAB_COUNT 5
#include "afxcmn.h"

#include "Tools/TimeDlg.h"
#include "Tools/IPAddress.h"
// CConvertToolsDlg 对话框
class CConvertToolsDlg : public CDialog
{
// 构造
public:
	CConvertToolsDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CConvertToolsDlg();
// 对话框数据
	enum { IDD = IDD_CONVERTTOOLS_DIALOG };
public:
	void ShowSelectSel(UINT uSelectedSel);
	BOOL CreateTabSel(CDialog* dlg,UINT nIDTemplate);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
	CTabCtrl m_TabCtrl;
	CDialog *m_pToolDlg[TAB_COUNT];
};
