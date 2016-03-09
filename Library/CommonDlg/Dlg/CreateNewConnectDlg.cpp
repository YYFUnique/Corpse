// CreateNewConnectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../CommonDlg.h"
#include "CreateNewConnectDlg.h"

// CCreateNewConnectDlg �Ի���
IMPLEMENT_DYNAMIC(CCreateNewConnectDlg, CDialog)

CCreateNewConnectDlg::CCreateNewConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateNewConnectDlg::IDD, pParent)
{

}

CCreateNewConnectDlg::CCreateNewConnectDlg(LPCTSTR lpszIPaddr,LPCTSTR lpszPort)
	: CDialog(CCreateNewConnectDlg::IDD,NULL)
{

}

CCreateNewConnectDlg::~CCreateNewConnectDlg()
{

}

void CCreateNewConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCreateNewConnectDlg, CDialog)

END_MESSAGE_MAP()


// CCreateNewConnectDlg ��Ϣ�������
BOOL CCreateNewConnectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_IPADDRESS)->SetFocus();
#ifdef _DEBUG
	GetDlgItem(IDC_IPADDRESS)->SetWindowText(_T("127.0.0.1"));
	GetDlgItem(IDC_EDIT_PORT)->SetWindowText(_T("7890"));
#endif

	return TRUE;
}

void CCreateNewConnectDlg::OnOK()
{
	GetDlgItemText(IDC_IPADDRESS,m_strIPAddr);
	GetDlgItemText(IDC_EDIT_PORT,m_strPort);
	
	int nID = 0;
	if (m_strIPAddr.CompareNoCase(_T("0.0.0.0")) == 0)
	{
		nID = IDC_IPADDRESS;
		MessageBox(_T("������Զ��IP��ַ"),_T("��ʾ"),MB_OK);
	}
	else if (m_strPort.IsEmpty())
	{
		nID = IDC_EDIT_PORT;
		MessageBox(_T("������Զ�̶˿ں�"),_T("��ʾ"),MB_OK);
	}
	if (nID != 0)
	{
		GetDlgItem(nID)->SetFocus();
		return;
	}

	CDialog::OnOK();
}

CString& CCreateNewConnectDlg::GetIPAddr()
{
	return m_strIPAddr;
}

CString& CCreateNewConnectDlg::GetPort()
{
	return m_strPort;
}