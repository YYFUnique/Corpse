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
	void OnBtnBeep(TNotifyUI& msg);
	void OnBtnPopCard(TNotifyUI& msg);
	void OnSetPort(TNotifyUI& msg);
	void OnNewKey(TNotifyUI& msg, int nNewKey = 1);
	void OnReadCard(TNotifyUI& msg);
	void OnEraseCard(TNotifyUI& msg);
	void OnBtnLost(TNotifyUI& msg);
	void OnBtnCheckOut(TNotifyUI& msg);
	void OnChangeUser(TNotifyUI& msg);
	void OnReadCardId(TNotifyUI& msg);
	void OnReadData(TNotifyUI& msg);
	void OnWriteData(TNotifyUI& msg);
	void OnBtnGetBufInfo(TNotifyUI& msg);
	void OnBtnGetCardInfo(TNotifyUI& msg);
protected:
	void GetCardType(CARD_TYPE CardType, CDuiString& strTipInfo);
	void GetCardState(CARD_STATE CardState, CDuiString& strTipInfo);
private:
	CADELLock* m_pADELLock;
};