#pragma once
#include "../Resource.h"
#include "afxcmn.h"
#include "afxwin.h"

class CCreateNewConnectDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateNewConnectDlg)

public:
	CCreateNewConnectDlg(CWnd* pParent = NULL);   // 标准构造函数
	CCreateNewConnectDlg(LPCTSTR lpszIPaddr,LPCTSTR lpszPort);
	virtual ~CCreateNewConnectDlg();

// 对话框数据
	enum { IDD = IDD_CREATE_NEW_CONNECT_DLG };
public:
	CString& GetIPAddr();
	CString& GetPort();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
protected:
	CString m_strIPAddr;
	CString m_strPort;
};
