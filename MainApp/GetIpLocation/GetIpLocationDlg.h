
// GetIpLocationDlg.h : 头文件
//

#pragma once

// CGetIpLocationDlg 对话框
class CGetIpLocationDlg : public CDialog
{
// 构造
public:
	CGetIpLocationDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CGetIpLocationDlg();
// 对话框数据
	enum { IDD = IDD_GETIPLOCATION_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOk();
	BOOL FillTheEditBox(LPCTSTR lpszJson);
};
