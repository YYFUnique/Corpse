// ToolSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CopyFile.h"
#include "ToolSettingDlg.h"


// CToolSettingDlg 对话框

IMPLEMENT_DYNAMIC(CToolSettingDlg, CDialog)

CToolSettingDlg::CToolSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolSettingDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CToolSettingDlg::~CToolSettingDlg()
{
}

void CToolSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CToolSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CToolSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CToolSettingDlg 消息处理程序
BOOL CToolSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);

	TCHAR szIniFilePath[MAX_PATH];
	GetModuleFileName(NULL , szIniFilePath , MAX_PATH);
	PathRenameExtension(szIniFilePath , _T(".ini"));

	UINT nCover = GetPrivateProfileInt(_T("BaseInfo") , _T("CoverWhenEver") , 0 , szIniFilePath);
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_COVER);
	pCheckBox->SetCheck(nCover);

	return TRUE;
}
void CToolSettingDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szIniFilePath[MAX_PATH];
	GetModuleFileName(NULL , szIniFilePath , MAX_PATH);
	PathRenameExtension(szIniFilePath , _T(".ini"));

	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_COVER);	
	int nState = pCheckBox->GetCheck();
	CString strCheckState;
	strCheckState.Format(_T("%d"),nState);
	WritePrivateProfileString(_T("BaseInfo") , _T("CoverWhenEver"),strCheckState,szIniFilePath);

	OnOK();
}
