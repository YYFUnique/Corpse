#pragma once

#include "LsCommon/TrueColorToolBar.h"
#include "NetTransport/WinSocket.h"
#include "ClientAppView.h"
class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:
	CString& GetMainAppTitle();
	void SetMainAppTitle(LPCTSTR lpszMainAppTitle);
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	BOOL SendData(CONNID dwConnID, PMSG_HEAD pData, int iLength);

	static ISocketListener::EnHandleResult SendData(CONNID dwConnID, const BYTE* pData, int iLength);
	static ISocketListener::EnHandleResult ReceiveData(CONNID dwConnID, const BYTE* pData, int iLength);
	static ISocketListener::EnHandleResult CloseSocket(CONNID dwConnID);
	static ISocketListener::EnHandleResult AcceptClient(CONNID dwConnID, UINT_PTR soClient);
// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CServerSocket& GetServer();
protected:  // 控件条嵌入成员
	CStatusBar				m_wndStatusBar;
	CTrueColorToolBar	m_wndToolBar;
	CString m_strMainAppTitle;
	CServerSocket m_Server;
protected:
	void InsertItem(DWORD dwConnID,CMsgHead& Msg);
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	/************************************************************************/
	/* 函数功能：当鼠标在Toolbar上面时显示提示文字                                     */
	/************************************************************************/
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnToolbarClick(UINT nID);

	afx_msg void OnStartServerDlg();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
};


