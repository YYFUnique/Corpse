
// TestSendRecvByteDlg.h : ͷ�ļ�
//

#pragma once


// CTestSendRecvByteDlg �Ի���
class CTestSendRecvByteDlg : public CDialog
{
// ����
public:
	CTestSendRecvByteDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTSENDRECVBYTE_DIALOG };

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
};
