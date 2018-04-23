#pragma once

// TrayData;
//TrayData.Pos = TrayArea;
//TrayData.strTitle = pTrayData.szTip;
//TrayData.hWnd = pTrayData.hwnd;
//TrayData.hTrayIcon = SetTrayIcon(pTrayData.hIcon);
//TrayData.uID = pTrayData.uID;
//TrayData.uBtnID = TbBtn.idCommand;
//TrayData.uCallbackMessage = pTrayData.uCallbackMessage;
//TrayData.bShowTray = (TbBtn.fsState & TBSTATE_HIDDEN) == TBSTATE_HIDDEN ? TRUE : FALSE;

typedef struct tagTRAYDATAFIREWALL
{
	HWND		hMainWnd;
	UINT		uID;
	UINT		uCallbackMessage;	
	DWORD	Reserved[2];		
	HICON		hIcon;								//����ͼ��ľ��	
	DWORD	Reserved2[3];
	TCHAR		szExePath[MAX_PATH];
	TCHAR		szTip[128];
}TRAYDATAFIREWALL,*PTRAYDATAFIREWALL;

typedef struct tagTRAYDATAFIREWALL64
{
	INT64		hMainWnd;
	UINT		uID;
	UINT		uCallbackMessage;	
	DWORD	Reserved[2];
	HICON		hIcon;								//����ͼ��ľ��	
	DWORD	Reserved2[5];
	TCHAR		szExePath[MAX_PATH];
	TCHAR		szTip[128];
}TRAYDATAFIREWALL64,*PTRAYDATAFIREWALL64;

typedef enum _TRAY_AREA
{
	Tray = 0,
	Overflow,
}TRAY_AREA;

typedef struct tagTRAYDATAINFO
{
	CString		strTitle;						//������ʾ����
	CString		strFilePath;					//	�ļ�·��
	HICON		hTrayIcon;					//����ͼ��
	HWND		hWnd;							//���ھ��
	UINT		uID;								//ͼ��ID
	UINT		uBtnID;
	UINT		uCallbackMessage;		//�ص�������Ϣ��
	BOOL		bShowTray;					//�Ƿ���ʾͼ��
	UINT		bTrayIcon;					//����ͼ��
	TRAY_AREA TrayArea;
}TRAYDATAINFO,*PTRAYDATAINFO;

typedef CDuiList<TRAYDATAINFO,const TRAYDATAINFO&> CTrayDataInfo;

typedef struct _TBBUTTON64 {
	int iBitmap;
	int idCommand;
	BYTE fsState;
	BYTE fsStyle;
	BYTE bReserved[6];          // padding for alignment
	DWORD_PTR dwData;
	INT_PTR iString;
} TBBUTTON64, NEAR* PTBBUTTON64;

class CTrayIcon : public CNotifyPump
{
public:
	CTrayIcon();
	~CTrayIcon();

public:
	void OnTrayMenu(CControlUI* pControl);
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
protected:
	DUI_DECLARE_MESSAGE_MAP();
	void OnLoadItem(TNotifyUI& msg);
	void OnRefresh(TNotifyUI& msg);
	void OnMenu(TNotifyUI& msg);
protected:

	BOOL GetTrayAreaIcon(CTrayDataInfo& TrayDataInfo);
	BOOL GetOverflowAreaIcon(CTrayDataInfo& TrayDataInfo);
	BOOL GetTrayIcon(HWND hWndTray, CTrayDataInfo& TrayDataInfoList);
	BOOL GetTrayIcon64(HWND hWndTray, CTrayDataInfo& TrayDataInfoList);
private:
	BOOL					   m_bLoad;
	CControlBuilder	   m_RootBuilder;
	CDialogBuilder	   m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};