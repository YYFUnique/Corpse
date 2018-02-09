#pragma once

class CTxPCMain : public WindowImplBase
{
public:
	CTxPCMain();
	~CTxPCMain();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);

protected:
	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
protected:
	static void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

private:
	static int m_nMoveNumber;
};