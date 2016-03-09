
// NetShareMgrDlg.h : ͷ�ļ�
//

#pragma once

typedef struct _LS_NET_SHARE_INFO
{
	CString strNetName;		//����Ŀ¼��
	DWORD dwType;			//����Ŀ¼�����ͣ����̡�IPC����ӡ����
	CString strRemark;
	DWORD dwPermissions;
	DWORD dwMaxUses;
	DWORD dwCurrentUses;
	CString strFilePath;			//����Ŀ¼��ַ
	CString strPassword;
}LS_NET_SHARE_INFO,*PLS_NET_SHARE_INFO;

typedef CArray<LS_NET_SHARE_INFO , LS_NET_SHARE_INFO&> CLsNetShareInfoArray;

// CNetShareMgrDlg �Ի���
class CNetShareMgrDlg : public CDialog
{
// ����
public:
	CNetShareMgrDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NETSHAREMGR_DIALOG };

	protected:
		
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	BOOL EnumLocalNetShare(CLsNetShareInfoArray& NetShareInfoArray);
	BOOL ShowNetShare();
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
