
// TextConvertDlg.h : ͷ�ļ�
//

#pragma once


// CTextConvertDlg �Ի���
class CTextConvertDlg : public CDialog
{
// ����
public:
	CTextConvertDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TEXTCONVERT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	BOOL ShowAnsiText();
	BOOL ShowUnicodeText();
	BOOL ShowUtf8Text();
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditTextContent();
	afx_msg void OnRadioClick(UINT nID);
};
