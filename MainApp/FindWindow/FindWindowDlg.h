
// FindWindowDlg.h : 头文件
//

#pragma once
#include "MessageCode.h"
#include "afxwin.h"
#include "HandlePic.h"

// CFindWindowDlg 对话框
class CFindWindowDlg : public CDialog
{
// 构造
public:
	CFindWindowDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FINDWINDOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual void PostNcDestroy();

// 实现
protected:
	HICON m_hIcon;
	NOTIFYICONDATA NotifyData;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnExitMainframe();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSyncPaint();
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
