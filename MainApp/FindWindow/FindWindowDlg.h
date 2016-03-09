
// FindWindowDlg.h : ͷ�ļ�
//

#pragma once
#include "MessageCode.h"
#include "afxwin.h"
#include "HandlePic.h"

// CFindWindowDlg �Ի���
class CFindWindowDlg : public CDialog
{
// ����
public:
	CFindWindowDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FINDWINDOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual void PostNcDestroy();

// ʵ��
protected:
	HICON m_hIcon;
	NOTIFYICONDATA NotifyData;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnExitMainframe();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNotiyfIconMessage( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

	void ShowMainFrameWindow();
public:
	void ShowWindowData(HWND hwnd);
	void CFindWindowDlg::LoadStringById(int nStringId , TCHAR* pszBuffer , int iSizeBuff);

	//CStatic m_StaticPic;
	CHandlePic m_HandlePic;
	afx_msg void OnShowMainframe();
	afx_msg void OnMenuSystemSetting();
private:
	HWND m_OtherWnd;
public:
	afx_msg void OnBnClickedCheckShowNumWay();
	afx_msg void OnClose();
};
