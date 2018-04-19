#pragma once

typedef struct _IPHeader // 20�ֽڵ�IPͷ 
{ 
	UCHAR	nVerLen;	// �汾�ź�ͷ���ȣ���ռ4λ�� 
	UCHAR	ipTOS;		// �������� 
	USHORT	nLen;		// ����ܳ��ȣ�������IP���ĳ��� 
	USHORT	nID;			// �����ʶ��Ωһ��ʶ���͵�ÿһ�����ݱ� 
	USHORT	nFlags;		// ��־ 
	UCHAR	nTTL;		// ����ʱ�䣬����TTL 
	UCHAR	nProtocol; // Э�飬������TCP��UDP��ICMP�� 
	USHORT	nCrc; // У��� 
	ULONG	nSrc; // ԴIP��ַ 
	ULONG	nDst; // Ŀ��IP��ַ 
} IPHeader, *PIPHeader; 

typedef struct _UDPHeader 
{ 
	USHORT         SrcPort;         // Դ�˿ں� 
	USHORT         DstPort;		// Ŀ�Ķ˿ں� 
	USHORT         nLen;            // ������� 
	USHORT         nCrc;            // У��� 
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
