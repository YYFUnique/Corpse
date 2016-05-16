#pragma once

class CIPTools : public WindowImplBase
{
public:
	CIPTools(HWND hWnd);
	~CIPTools();
public:
		
	virtual void InitWindow();
	void OnFinalMessage(HWND hWnd);
	void Notify(TNotifyUI& msg);
	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetWindowClassName(void) const;

	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

protected:
	void OnClick(TNotifyUI& msg);
	void OnTextChanged(TNotifyUI& msg);

	
	void OnIPConvernt(TNotifyUI& msg);
	void SetIPInfo(CControlUI* pControl, int dwIP);
	void ClearIPInfo(CControlUI* pControl);

	void OnNetAddr(TNotifyUI& msg);
	void ClearNetInfo();

	void OnMaskCalc(TNotifyUI& msg);
	void SetMaskInfo(CControlUI* pControl, int nCount);
	void ClearMaskInfo(CControlUI* pControl);
};