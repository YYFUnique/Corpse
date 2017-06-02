#pragma once

class CTxMiniSkin : public WindowImplBase
{
public:
	CTxMiniSkin(HWND hParent);
	~CTxMiniSkin();

	void OnFinalMessage( HWND hWnd );
	virtual LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	UILIB_RESOURCETYPE GetResourceType() const;
	void InitWindow();

	void Notify(TNotifyUI& msg);

public:
	static BOOL SetAutoStart(BOOL bAutoStart);

protected:
	void OnClick(TNotifyUI& msg);
	void OnChanged(TNotifyUI& msg);
protected:
	//HWND m_hParent;
};