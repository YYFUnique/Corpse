#include "stdafx.h"
#include "ShutDownSystemDlg.h"


// CShutDownSystemDlg 对话框

IMPLEMENT_DYNAMIC(CShutDownSystemDlg, CDialog)

CShutDownSystemDlg::CShutDownSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShutDownSystemDlg::IDD, pParent)
{
	m_hIcon = LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_SHUT_DOWN));
}

CShutDownSystemDlg::~CShutDownSystemDlg()
{
}

void CShutDownSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShutDownSystemDlg, CDialog)
END_MESSAGE_MAP()


// CShutDownSystemDlg 消息处理程序
BOOL CShutDownSystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);


	return TRUE;
}