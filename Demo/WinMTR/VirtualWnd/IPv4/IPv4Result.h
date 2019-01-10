#pragma once
#include "DllCore/Thread/LsThread.h"

typedef struct tagHOST_INFO 
{
	CString strHostName;			// 主机域名、IP地址
	DWORD dwCount;				// 发送Ping包次数
	DWORD dwInterval;			// 每次发送时间间隔
	DWORD dwTTL;					// 数据包存活值
	DWORD dwPacketSize;		// 数据包大小
	DWORD dwTimeout;			// 超时时间
}HOST_INFO, *PHOST_INFO;

class CIPv4Result : public CNotifyPump, public CLsThread
{
public:
	CIPv4Result();
	~CIPv4Result();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);
	void SetVirtualWnd(LPCTSTR lpszVirtualWnd);
	LPCTSTR GetVirtualWnd() const;
	void NotifyTask(PCNTCHDR pNTCHDR);
protected:
	BOOL IsIPv4String(LPCTSTR lpszIpString);
protected:
	//默认线程函数
	virtual UINT ThreadProc();
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnLoadItem(TNotifyUI& msg);
		void	OnClick(TNotifyUI& msg);
		void OnStop();
private:
	BOOL m_bLoad;
	HOST_INFO*		m_pHostInfo;
	CString					m_strVirtualWnd;
	CControlBuilder	m_RootBuilder;
	CDialogBuilder	m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};