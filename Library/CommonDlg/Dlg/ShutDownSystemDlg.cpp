#include "stdafx.h"
#include "ShutDownSystemDlg.h"


// CShutDownSystemDlg �Ի���

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


// CShutDownSystemDlg ��Ϣ�������
BOOL CShutDownSystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);


	return TRUE;
}