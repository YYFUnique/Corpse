#pragma once


// CToolSettingDlg 对话框

class CToolSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolSettingDlg)

public:
	CToolSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolSettingDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_TOOL_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	HICON m_hIcon;
public:
	afx_msg void OnBnClickedOk();
};
