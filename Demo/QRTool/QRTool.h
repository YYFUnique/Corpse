#pragma once

typedef struct _tagQRCodeItem 
{
	DWORD dwLevel;						//二维码纠错级别
	DWORD dwSize;							//二维码尺寸
	DWORD dwMag;							//放大倍数
	DWORD dwBorder;						//边缘留白
	CDuiString strQrCode;						//二维码内容
}QRCodeItem,*PQRCodeItem;

#define WM_QRCODEITEM_INFO			(WM_USER+0x1000)			//二维码更新消息

class CQRTool : public WindowImplBase
{
public:
	CQRTool();
	~CQRTool();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);

	CControlUI* CreateControl(LPCTSTR pstrClass);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	void OnSave();
	void OnClick(TNotifyUI& msg);
	void OnCreate(TNotifyUI& msg);
	LRESULT OnQRCodeItemInfo(WPARAM wParam, LPARAM lParam);
};