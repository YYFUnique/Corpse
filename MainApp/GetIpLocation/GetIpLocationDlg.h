
// GetIpLocationDlg.h : ͷ�ļ�
//

#pragma once

// CGetIpLocationDlg �Ի���
class CGetIpLocationDlg : public CDialog
{
// ����
public:
	CGetIpLocationDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CGetIpLocationDlg();
// �Ի�������
	enum { IDD = IDD_GETIPLOCATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	
	
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOk();
	BOOL FillTheEditBox(LPCTSTR lpszJson);
};
