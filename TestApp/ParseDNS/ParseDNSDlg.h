
// ParseDNSDlg.h : ͷ�ļ�
//

#pragma once


// CParseDNSDlg �Ի���
class CParseDNSDlg : public CDialog
{
// ����
public:
	CParseDNSDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CParseDNSDlg();
// �Ի�������
	enum { IDD = IDD_PARSEDNS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

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
// ʵ��
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
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBtnStartClicked();
	afx_msg void OnBtnStopClicked();

	afx_msg LRESULT OnShowTrayMessage(WPARAM wParam,LPARAM lParam);
};
