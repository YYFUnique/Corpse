#pragma once

typedef struct _IPHeader // 20字节的IP头 
{ 
	UCHAR	nVerLen;	// 版本号和头长度（各占4位） 
	UCHAR	ipTOS;		// 服务类型 
	USHORT	nLen;		// 封包总长度，即整个IP报的长度 
	USHORT	nID;			// 封包标识，惟一标识发送的每一个数据报 
	USHORT	nFlags;		// 标志 
	UCHAR	nTTL;		// 生存时间，就是TTL 
	UCHAR	nProtocol; // 协议，可能是TCP、UDP、ICMP等 
	USHORT	nCrc; // 校验和 
	ULONG	nSrc; // 源IP地址 
	ULONG	nDst; // 目的IP地址 
} IPHeader, *PIPHeader; 

typedef struct _UDPHeader 
{ 
	USHORT         SrcPort;         // 源端口号 
	USHORT         DstPort;		// 目的端口号 
	USHORT         nLen;            // 封包长度 
	USHORT         nCrc;            // 校验和 
} UDPHeader, *PUDPHeader; 

class CWakeOnLan : public WindowImplBase
{
public:
	CWakeOnLan(HWND hParent);
	~CWakeOnLan();

	virtual void InitWindow();
	void OnFinalMessage(HWND hWnd);
	void Notify(TNotifyUI& msg);

	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetWindowClassName(void) const;

	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
protected:
	void OnClick(TNotifyUI& msg);
	void OnWOLSend(TNotifyUI& msg);
	void OnItemSelect(TNotifyUI& msg);
private:
	TCHAR m_szHostNameFilePath[MAX_PATH];
};
