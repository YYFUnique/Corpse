#pragma once


// CSystemSet �Ի���

class CSystemSet : public CDialog
{
	DECLARE_DYNAMIC(CSystemSet)

public:
	CSystemSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSystemSet();

// �Ի�������
	enum { IDD = IDD_DIALOG_SYSTEM_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedBtnSave();
protected:
};
