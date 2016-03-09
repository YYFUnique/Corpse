
// ParseDNSDlg.h : 头文件
//

#pragma once


// CParseDNSDlg 对话框
class CParseDNSDlg : public CDialog
{
// 构造
public:
	CParseDNSDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CParseDNSDlg();
// 对话框数据
	enum { IDD = IDD_PARSEDNS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	void CreateTrayIcon();
	static UINT ParseDNS(LPARAM lParam);
	static UINT ConnectRemoteIP(LPARAM lParam);
	BOOL IsDetectOver();
	int GetPeriod();
	CString GetRemoteDname();
	CString GetRemoteIp();
	void  ShowTrayIconTextTip(LPCTSTR lpszTitle,LPCTSTR lpszTooltip);
	void EnableWindowControl(BOOL bEnable);
	HANDLE GetEventHandle();
// 实现
protected:
	HICON m_hIcon;
	CEdit*    m_pEditCtrl;
	CString	  m_strRemoteDname;
	CString	  m_strRemoteIp;
	HANDLE m_hEvent;
	HANDLE m_hThreadRemoteDname;
	HANDLE m_hThreadRemoteIp;
	int          m_nPeriod;
	BOOL	  m_bDetecteOver;
	NOTIFYICONDATA m_trayData;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBtnStartClicked();
	afx_msg void OnBtnStopClicked();

	afx_msg LRESULT OnShowTrayMessage(WPARAM wParam,LPARAM lParam);
};
