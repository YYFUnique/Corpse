#pragma once

class CPostman : public WindowImplBase
{
public:
	CPostman();
	~CPostman();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);

protected:
	void OnSize(TNotifyUI& msg);
	void OnClick(TNotifyUI& msg);
	void ShowBar(BOOL bShowBar);
	
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	BOOL m_bShowBar;
};