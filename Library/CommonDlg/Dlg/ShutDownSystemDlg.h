#pragma once
#include "../Resource.h"
// CShutDownSystemDlg 对话框

class CShutDownSystemDlg : public CDialog
{
	DECLARE_DYNAMIC(CShutDownSystemDlg)

public:
	CShutDownSystemDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShutDownSystemDlg();

// 对话框数据
	enum { IDD = IDD_SHUT_DOWN_DLG };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
