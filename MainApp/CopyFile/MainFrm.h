
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

#include "SelfStruct.h"

#define  WM_UPDATE_STSTUS (WM_USER + 0x110)

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	MODIFY_SAVE m_ModyfySave;	
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdatePane(CCmdUI* pCmdUI);
	afx_msg void OpenIniFile();
	afx_msg void CopyFileAgain();
	afx_msg void OnAbout();
	afx_msg LRESULT SetStatusBarValue(WPARAM wParam , LPARAM lParam);
	afx_msg void AddIniRule();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void SaveAllToIniFile();
	void ChangeModifyStatus(MODIFY_SAVE MDSave=NOT_MODIFY);
	//void SetStatusBarValue(LPCTSTR StatusBarValue);
	afx_msg void OnMenuOpenPath();
	afx_msg void OnCopyFileSetting();
	void GetTargetFilePath(LPTSTR lpszTargetFilePath , DWORD dwSize);
	BOOL SetTargetFilePath(LPCTSTR lpszTargetFilePath,LPCTSTR lpszIniFilePath);
};


