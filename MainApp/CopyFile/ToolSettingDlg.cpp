// ToolSettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CopyFile.h"
#include "ToolSettingDlg.h"


// CToolSettingDlg �Ի���

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


// CToolSettingDlg ��Ϣ�������
BOOL CToolSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
