#pragma once

class CPlayerMain : public WindowImplBase
{
public:
	CPlayerMain();
	~CPlayerMain();
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	void InitWindow();
	CControlUI* CreateControl(LPCTSTR pstrClass);
	void AddItemToList(LPCTSTR lpszTitle,LPCTSTR lpszType,LPCTSTR lpszSize,LPCTSTR lpszModifyTime);

protected:
	CTreeViewUI * m_pTreeView;
	CListUI*			 m_pListView;
	CComboUI*		 m_pCombo;
	
	//DUI_DECLARE_MESSAGE_MAP()
	void CPlayerMain::OnClick(TNotifyUI& msg);
};