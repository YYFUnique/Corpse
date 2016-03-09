
// CreateMacMapDlg.h : 头文件
//

#pragma once
typedef CMap<DWORD,DWORD,CString,LPCTSTR>CMacAddressMap;

// CCreateMacMapDlg 对话框
class CCreateMacMapDlg : public CDialog
{
// 构造
public:
	CCreateMacMapDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CREATEMACMAP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CMacAddressMap m_MacAddressMap;
	TCHAR					   m_MacAddressFilePath[MAX_PATH];
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnSave();
	LRESULT OnLoadMacAddress(WPARAM wParam,LPARAM lParam);
};
