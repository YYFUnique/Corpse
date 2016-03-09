
// DriverToolDlg.h : ͷ�ļ�
//

#pragma once


// CDriverToolDlg �Ի���
class CDriverToolDlg : public CDialog
{
// ����
public:
	CDriverToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DRIVERTOOL_DIALOG };

	BOOL GetEnvironmentInstallPath(CString& strWinDDKInstallPath,CString& strVCInstallPath);
	BOOL GetWinDDKInstallPath(CString& strWinDDKInstallPath);
	void ShowSelectFolder(CString& strInstallPath);

	BOOL GetVCInstallPath(CString& strVCInstallPath);
	BOOL CopyFileToDestation(LPCTSTR lpszExistFileName,LPCTSTR lpszFileNewName);
	BOOL LookUpDirectory(LPCTSTR lpszExistFileName,LPCTSTR lpszFileNewName);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnInstallGuide();
	afx_msg void OnBnClickedBtnUninstallGuide();
};
