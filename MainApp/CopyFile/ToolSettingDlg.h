#pragma once


// CToolSettingDlg �Ի���

class CToolSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolSettingDlg)

public:
	CToolSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CToolSettingDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_TOOL_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	HICON m_hIcon;
public:
	afx_msg void OnBnClickedOk();
};
