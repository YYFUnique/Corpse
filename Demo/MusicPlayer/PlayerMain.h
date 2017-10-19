#pragma once

#include "DuiLib/Utils/UITrayIcon.h"

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
	
	void InitWindow();
	CControlUI* CreateControl(LPCTSTR pstrClass);
	
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	//DUI_DECLARE_MESSAGE_MAP()
	void OnClick(TNotifyUI& msg);
	LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);
	void AddItemToList(LPCTSTR lpszTitle,LPCTSTR lpszType,LPCTSTR lpszSize,LPCTSTR lpszModifyTime);
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
protected:
	CTreeViewUI * m_pTreeView;
	CListUI*			 m_pListView;
	CComboUI*		 m_pCombo;
	CUITrayIcon*	 m_pTrayIcon;
};