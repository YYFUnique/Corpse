
// TextConvertDlg.h : 头文件
//

#pragma once


// CTextConvertDlg 对话框
class CTextConvertDlg : public CDialog
{
// 构造
public:
	CTextConvertDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TEXTCONVERT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	BOOL ShowAnsiText();
	BOOL ShowUnicodeText();
	BOOL ShowUtf8Text();
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditTextContent();
	afx_msg void OnRadioClick(UINT nID);
};
