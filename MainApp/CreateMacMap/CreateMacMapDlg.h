
// CreateMacMapDlg.h : ͷ�ļ�
//

#pragma once
typedef CMap<DWORD,DWORD,CString,LPCTSTR>CMacAddressMap;

// CCreateMacMapDlg �Ի���
class CCreateMacMapDlg : public CDialog
{
// ����
public:
	CCreateMacMapDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CREATEMACMAP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CMacAddressMap m_MacAddressMap;
	TCHAR					   m_MacAddressFilePath[MAX_PATH];
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnSave();
	LRESULT OnLoadMacAddress(WPARAM wParam,LPARAM lParam);
};
