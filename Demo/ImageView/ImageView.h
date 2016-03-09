#pragma once

class CImageView : public WindowImplBase
{
public:
	CImageView();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual void Notify(TNotifyUI& msg);
	void InitWindow();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam);
	LRESULT OnWindowPosChanged(WPARAM wParam, LPARAM lParam);
	CControlUI* CreateControl(LPCTSTR pstrClass);
protected:
	void OnClick(TNotifyUI& msg);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	UINT m_nLastState;
};