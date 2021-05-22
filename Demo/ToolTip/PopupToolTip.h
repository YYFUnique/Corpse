#pragma once

class CPopupToolTip : public WindowImplBase
{
public:
	CPopupToolTip();
	~CPopupToolTip();

public:
	virtual void OnFinalMessage(HWND hWnd);
	virtual LPCTSTR GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
public:
	void Show(BOOL bShow);
	void SetTooltip(LPCTSTR lpszTooltip);
protected:
	void OnClick(TNotifyUI& msg);
	void OnPopup(TNotifyUI& msg);
};