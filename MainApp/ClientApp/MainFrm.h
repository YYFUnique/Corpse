#pragma once

#include "LsCommon/TrueColorToolBar.h"
#include "NetTransport/WinSocket.h"
#include "ClientAppView.h"
class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:
	CString& GetMainAppTitle();
	void SetMainAppTitle(LPCTSTR lpszMainAppTitle);
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	BOOL SendData(CONNID dwConnID, PMSG_HEAD pData, int iLength);

	static ISocketListener::EnHandleResult SendData(CONNID dwConnID, const BYTE* pData, int iLength);
	static ISocketListener::EnHandleResult ReceiveData(CONNID dwConnID, const BYTE* pData, int iLength);
	static ISocketListener::EnHandleResult CloseSocket(CONNID dwConnID);
	static ISocketListener::EnHandleResult AcceptClient(CONNID dwConnID, UINT_PTR soClient);
// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CServerSocket& GetServer();
protected:  // �ؼ���Ƕ���Ա
	CStatusBar				m_wndStatusBar;
	CTrueColorToolBar	m_wndToolBar;
	CString m_strMainAppTitle;
	CServerSocket m_Server;
protected:
	void InsertItem(DWORD dwConnID,CMsgHead& Msg);
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	/************************************************************************/
	/* �������ܣ��������Toolbar����ʱ��ʾ��ʾ����                                     */
	/************************************************************************/
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnToolbarClick(UINT nID);

	afx_msg void OnStartServerDlg();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
};


