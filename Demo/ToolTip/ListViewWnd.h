#pragma once
#include "DllCore/Window/Desktop.h"
#define LIST_VIEW_WND	_T("ListViewWnd")

class CListViewWnd : public WindowImplBase
{
public:
	CListViewWnd(HWND hParent, CEditUI2* pOwner);
	~CListViewWnd();
public:
	virtual void OnFinalMessage(HWND hWnd);
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR CListViewWnd::GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
protected:
	void OnItemSelect(TNotifyUI& msg);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	void Add(const DESKTOP_INFO& DesktopInfo);
	void RemoveAll();
	void ResizeWnd();
	LRESULT OnMouseWheel(UINT , WPARAM , LPARAM , BOOL& bHandle);
	LRESULT OnPosChanged(UINT , WPARAM , LPARAM , BOOL& bHandle);
	LRESULT OnKeyDown(UINT , WPARAM , LPARAM , BOOL& bHandled);
public:
	BOOL			m_bAutoClose;
	CEditUI2*		m_pOwner;
	CListUI*		m_pSearch;
	CDialogBuilder m_DialogBuilder;
};