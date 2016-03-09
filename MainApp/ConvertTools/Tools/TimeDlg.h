
// TimeDlg.h : 头文件
//

#pragma once
#include "../Resource.h"

// CTimeDlg 对话框
class CTimeDlg : public CDialog
{
	// 构造
public:
	CTimeDlg(CWnd* pParent = NULL);	// 标准构造函数
	void ShowMemberValue(FILETIME& FileTimePass,int nControlId=0,BOOL bLocalTime=FALSE);

	void UpdateControlData();
	void EnableAllWindow(BOOL bEnable);
	// 对话框数据
	enum { IDD = IDD_TIME_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	// 生成的消息映射函数
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CString m_CStyleTime;
	CString m_CStyleTimeLocal;
	CString m_SysTime;
	CString m_SysTimeLocal;
	CString m_FileTimeHigh;
	CString m_FileTimeLow;
	CString m_FileTime;
	CString m_FileTimeHighLocal;
	CString m_FileTimeLowLocal;
	CString m_FileTimeLocal;
public:
	afx_msg void OnBnClickedBtnGettime();
	afx_msg void OnEnChangeEditCstyleTime();
	afx_msg void OnEnChangeEdicsytyleTimeLoclal();
	afx_msg void OnEnChangeEditSystemTime();
	afx_msg void OnEnChangeEditSystemTimeLocal();
	afx_msg void OnEnChangeEditFtHigh();
	afx_msg void OnEnChangeEditFtLow();
	afx_msg void OnEnChangeEditFtHighLocal();
	afx_msg void OnEnChangeEditFtLowLocal();
	afx_msg void OnEnChangeEditFt();
	afx_msg void OnEnChangeEditFtLocal();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedCheckGetSystemTime();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
