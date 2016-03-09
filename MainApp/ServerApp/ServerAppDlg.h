#pragma once

#include "NetTransport/WinSocket.h"
#include "SystemInfo/ComputerInfo.h"

// CServerAppDlg 对话框
class CServerAppDlg : public CDialog
{
// 构造
public:
	CServerAppDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CServerAppDlg();
// 对话框数据
	enum { IDD = IDD_SERVERAPP_DIALOG };
	
	CClientSocket& GetClientSocket();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	static ISocketListener::EnHandleResult SendData(CONNID dwConnID, const BYTE* pData, int iLength);
	static ISocketListener::EnHandleResult ReceiveData(CONNID dwConnID, const BYTE* pData, int iLength);
	static ISocketListener::EnHandleResult CloseSocket(CONNID dwConnID);
	static ISocketListener::EnHandleResult ConnectOk(CONNID dwConnID,LPCTSTR lpszIPAddr,WORD wPort);
// 实现
protected:
	HICON m_hIcon;
	CClientSocket m_Client;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuExit();
	afx_msg void OnCreateNewConnectDlg();
	DECLARE_MESSAGE_MAP()
};
