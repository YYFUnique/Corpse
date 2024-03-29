#pragma once

#include "DllCore/Window/MagnetFrame.h"

typedef struct _tagQRCodeItem 
{
	DWORD dwLevel;						//��ά���������
	DWORD dwSize;							//��ά��ߴ�
	DWORD dwMag;							//�Ŵ���
	DWORD dwBorder;						//��Ե����
	CDuiString strQrCode;					//��ά������
	CDuiString strLogoFile;				//��ά������ͼƬ
}QRCodeItem,*PQRCodeItem;

#define WM_QRCODEITEM_INFO			(WM_USER+0x1000)			//��ά�������Ϣ

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
	CDuiString GetZIPFileName() const;
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	void OnSave();
	void OnClick(TNotifyUI& msg);
	void OnCreate(TNotifyUI& msg);
	LRESULT OnQRCodeItemInfo(WPARAM wParam, LPARAM lParam);

	LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
};