
// SendArpDlg.h : ͷ�ļ�
//

#pragma once


// CSendArpDlg �Ի���
class CSendArpDlg : public CDialog
{
// ����
public:
	CSendArpDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CSendArpDlg();
// �Ի�������
	enum { IDD = IDD_SENDARP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnSendArp(WPARAM wParam,LPARAM lParam);
};
