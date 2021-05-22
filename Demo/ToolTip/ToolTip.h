#pragma once

#include "PopupToolTip.h"
#include "ListViewWnd.h"

#include "DllCore/Window/Desktop.h"

class CToolTip : public WindowImplBase
{
public:
	CToolTip();
	~CToolTip();

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
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnClose(UINT , WPARAM , LPARAM , BOOL& bHandled);
protected:
	void OnClick(TNotifyUI& msg);
	void OnReturn(TNotifyUI& msg);
	void OnPopup(TNotifyUI& msg);
	void OnWindowInit(TNotifyUI& msg);
	void OnTextChanged(TNotifyUI& msg);
protected:
	BOOL GetDesktopList(HWND hDesktop, CDesktopInfoList& DesktopInfoList);
	BOOL GetDesktopIcon(HWND hDeskWnd, LPCTSTR lpszItemName, LPRECT lpRect, BOOL bVague = FALSE);
protected:
	CEditUI2*				m_pItemName;
	CPopupToolTip* m_pToolTip;
	CListViewWnd*	m_pListView;
	CDesktopInfoList m_DesktopInfoList;
};