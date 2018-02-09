#pragma once

class CSignTool : public WindowImplBase
{
public:
	CSignTool();
	~CSignTool();

public:
	virtual LPCTSTR GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);

protected:
	void OnSave(TNotifyUI& msg);
	void OnClick(TNotifyUI& msg);
	void OnLookup(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
};