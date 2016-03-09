
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

#include "SelfStruct.h"

#define  WM_UPDATE_STSTUS (WM_USER + 0x110)

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	MODIFY_SAVE m_ModyfySave;	
// ���ɵ���Ϣӳ�亯��
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


