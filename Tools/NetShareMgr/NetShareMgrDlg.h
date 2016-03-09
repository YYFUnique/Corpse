
// NetShareMgrDlg.h : 头文件
//

#pragma once

typedef struct _LS_NET_SHARE_INFO
{
	CString strNetName;		//共享目录名
	DWORD dwType;			//共享目录的类型，磁盘、IPC、打印机等
	CString strRemark;
	DWORD dwPermissions;
	DWORD dwMaxUses;
	DWORD dwCurrentUses;
	CString strFilePath;			//共享目录地址
	CString strPassword;
}LS_NET_SHARE_INFO,*PLS_NET_SHARE_INFO;

typedef CArray<LS_NET_SHARE_INFO , LS_NET_SHARE_INFO&> CLsNetShareInfoArray;

// CNetShareMgrDlg 对话框
class CNetShareMgrDlg : public CDialog
{
// 构造
public:
	CNetShareMgrDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_NETSHAREMGR_DIALOG };

	protected:
		
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	BOOL EnumLocalNetShare(CLsNetShareInfoArray& NetShareInfoArray);
	BOOL ShowNetShare();
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRClick(NMHDR* pNMHDR,LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnMenuRefresh();
	afx_msg void OnMenuDelete();
};
