#pragma once
#include "../Resource.h"
#include "afxcmn.h"
#include "afxwin.h"

class CCreateNewConnectDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateNewConnectDlg)

public:
	CCreateNewConnectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CCreateNewConnectDlg(LPCTSTR lpszIPaddr,LPCTSTR lpszPort);
	virtual ~CCreateNewConnectDlg();

// �Ի�������
	enum { IDD = IDD_CREATE_NEW_CONNECT_DLG };
public:
	CString& GetIPAddr();
	CString& GetPort();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
protected:
	CString m_strIPAddr;
	CString m_strPort;
};
