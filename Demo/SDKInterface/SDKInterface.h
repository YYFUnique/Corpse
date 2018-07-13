#pragma once

#include "ADELLock.h"

class CSDKInterface : public WindowImplBase
{
public:
	CSDKInterface();
	~CSDKInterface();

public:
	virtual LPCTSTR GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

public:
	void InitWindow();
	void Notify(TNotifyUI& msg);
	void OnFinalMessage( HWND hWnd );

protected:
	void OnClick(TNotifyUI& msg);
	void OnSelectedChanged(TNotifyUI& msg);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
protected:
	void OnInit(TNotifyUI& msg);
	void OnNewKey(TNotifyUI& msg, BOOL bNewKey = TRUE);
	void OnReadCard(TNotifyUI& msg);
	void OnEraseCard(TNotifyUI& msg);
	void OnChangeUser(TNotifyUI& msg);
	void OnReadCardId(TNotifyUI& msg);
	void OnReadData(TNotifyUI& msg);
	void OnWriteData(TNotifyUI& msg);
private:
	CADELLock* m_pADELLock;
};