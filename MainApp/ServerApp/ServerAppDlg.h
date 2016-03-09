#pragma once

#include "NetTransport/WinSocket.h"
#include "SystemInfo/ComputerInfo.h"

// CServerAppDlg �Ի���
class CServerAppDlg : public CDialog
{
// ����
public:
	CServerAppDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CServerAppDlg();
// �Ի�������
	enum { IDD = IDD_SERVERAPP_DIALOG };
	
	CClientSocket& GetClientSocket();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
private:
	static ISocketListener::EnHandleResult SendData(CONNID dwConnID, const BYTE* pData, int iLength);
	static ISocketListener::EnHandleResult ReceiveData(CONNID dwConnID, const BYTE* pData, int iLength);
	static ISocketListener::EnHandleResult CloseSocket(CONNID dwConnID);
	static ISocketListener::EnHandleResult ConnectOk(CONNID dwConnID,LPCTSTR lpszIPAddr,WORD wPort);
// ʵ��
protected:
	HICON m_hIcon;
	CClientSocket m_Client;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuExit();
	afx_msg void OnCreateNewConnectDlg();
	DECLARE_MESSAGE_MAP()
};
