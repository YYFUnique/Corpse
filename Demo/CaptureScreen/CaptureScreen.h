#pragma once

#include "DllCore/File/FileSystemWatcher.h"

class CCaptureScreen : public WindowImplBase
{
public:
	CCaptureScreen();
	~CCaptureScreen();

public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static void CALLBACK NotifyDeal(FILE_ACTION Action, LPCTSTR lpszFileName, LPVOID lParam);
protected:
	virtual void OnClick(TNotifyUI& msg);
	void OnInitDialog(TNotifyUI& msg);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	void OnWindowInit();
	BOOL CopyScreenToBitmap(LPRECT lpRect,HBITMAP& hDesktopMap,BOOL bSave = FALSE);
	BOOL CoverDesktopBmp(HBITMAP hDesktopMap,HBITMAP hCoverMap,HBITMAP& hCoverDesktopMap);

	
protected:
	void ShowScreen();
	void DrawTrackRect();

protected:
	HBITMAP m_hDesktopBitMap;
	HBITMAP m_hMask;

// 	CRectTracker* m_pRcTracker;
// 	CDuiRect m_RectSelected;
// 	CDuiPoint m_LastMousePt;

	CRectTrackerUI* m_pRcTracker;
	BOOL m_bDrawing;

	BOOL m_bHit;
	HCURSOR m_hColorCursor;

	CFileSystemWatcher FileObject;
};

