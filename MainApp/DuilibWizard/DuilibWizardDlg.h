
// DuilibWizardDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "WinErrorMsg.h"

typedef struct _tagVSVersionInfo
{
	CString strVersion;
	CString strVSInstallPath;
	CString strInstallTime;
}VSVersionInfo,*PVSVersionInfo;

typedef CList<VSVersionInfo,const VSVersionInfo&> CVSVersionInfoList;

// CDuilibWizardDlg �Ի���
class CDuilibWizardDlg : public CDialog
{
// ����
public:
	CDuilibWizardDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DUILIBWIZARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL GetVisualStudioVersion(CVSVersionInfoList& VersionInfoList);
	CString GetVSVersion(LPCTSTR szRegItemName);
	BOOL InstallDuilibWizard(LPCTSTR lpszVCProjectPath);
	BOOL CopyDirectory(LPCTSTR lpszSourceFilePath,LPCTSTR lpszTargetFilePath);
	afx_msg void OnBnClickedInstall();
	afx_msg void OnBnClickedUninstall();
protected:
	CListCtrl m_ListCtrl;
	CListBox m_ListBox;
	CVSVersionInfoList m_VersionInfoList;
	CWinErrorMsg* m_pErrorMsg;
};
