
// AnimatorDlg.h : ͷ�ļ�
//

#pragma once
#include "RichEditCtrlEx.h"
#include "afxcmn.h"

// CAnimatorDlg �Ի���
class CAnimatorDlg : public CDialog
{
// ����
public:
	CAnimatorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ANIMATOR_DIALOG };

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
	CRichEditCtrlEx m_RichEditEx;
public:
	afx_msg void OnBnClickedOk();
};
