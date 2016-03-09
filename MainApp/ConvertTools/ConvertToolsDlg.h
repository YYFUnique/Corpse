
// ConvertToolsDlg.h : ͷ�ļ�
//

#pragma once
#define TAB_COUNT 5
#include "afxcmn.h"

#include "Tools/TimeDlg.h"
#include "Tools/IPAddress.h"
// CConvertToolsDlg �Ի���
class CConvertToolsDlg : public CDialog
{
// ����
public:
	CConvertToolsDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CConvertToolsDlg();
// �Ի�������
	enum { IDD = IDD_CONVERTTOOLS_DIALOG };
public:
	void ShowSelectSel(UINT uSelectedSel);
	BOOL CreateTabSel(CDialog* dlg,UINT nIDTemplate);
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
	afx_msg void OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
	CTabCtrl m_TabCtrl;
	CDialog *m_pToolDlg[TAB_COUNT];
};
