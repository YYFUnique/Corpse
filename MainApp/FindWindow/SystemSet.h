#pragma once


// CSystemSet 对话框

class CSystemSet : public CDialog
{
	DECLARE_DYNAMIC(CSystemSet)

public:
	CSystemSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSystemSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_SYSTEM_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedBtnSave();
protected:
};
