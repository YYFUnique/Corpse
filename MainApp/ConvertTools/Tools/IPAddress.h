
#pragma once

#include "../Resource.h"
class CIpAddressDlg : public CDialog
{
	DECLARE_DYNAMIC(CIpAddressDlg)

public:
	CIpAddressDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIpAddressDlg();

	// 对话框数据
	enum { IDD = IDD_IPADDRESS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
private:
	// Ip地址
	CIPAddressCtrl m_IPAddr;
public:
	CEdit m_EditAddr;
	afx_msg void OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnCheckNetwork();
	void ShowMemberValue(DWORD dwIp,int nControlId);
	static UINT CheckNetWork(LPVOID lparam);
private:
	CListBox m_ListCtrl;
public:
	afx_msg void OnEnChangeEditIpAddr();
};
