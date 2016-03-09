#pragma once

class CExpressBus : public WindowImplBase
{
public:
	CExpressBus();
	~CExpressBus();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	//virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	void InitWindow();
	//CControlUI* CreateControl(LPCTSTR pstrClass);

	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);
		virtual void OnSelectChanged( TNotifyUI &msg );
		virtual void OnItemClick( TNotifyUI &msg );
};