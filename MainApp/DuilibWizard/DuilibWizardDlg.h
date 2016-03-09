
// DuilibWizardDlg.h : 头文件
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

// CDuilibWizardDlg 对话框
class CDuilibWizardDlg : public CDialog
{
// 构造
public:
	CDuilibWizardDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DUILIBWIZARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
