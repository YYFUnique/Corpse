#pragma once
#include "../Resource.h"
// CShutDownSystemDlg �Ի���

class CShutDownSystemDlg : public CDialog
{
	DECLARE_DYNAMIC(CShutDownSystemDlg)

public:
	CShutDownSystemDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShutDownSystemDlg();

// �Ի�������
	enum { IDD = IDD_SHUT_DOWN_DLG };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
