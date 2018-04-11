#pragma once
#include "PCHunter.h"
#include "PopupWidget.h"

class CSkinPickerDialog : public WindowImplBase
{
public:
	CSkinPickerDialog(LPCTSTR pszXMLPath, CPCHunter* pMainWnd);
	~CSkinPickerDialog();

public:
	LPCTSTR GetWindowClassName() const;
	void Notify(TNotifyUI& msg);

	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass) ;

protected:
	void OnClick(TNotifyUI& msg);
	void OnBtnTrans(TNotifyUI& msg);
	void OnChoseSkin(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
	void SetBkColor(DWORD dwBackColor);
	DWORD GetBkColor() const;
	void SetBkImage(CDuiString strBkImage);
	LPCTSTR GetBkImage();

	void OnBtnTrans(TNotifyUI& msg,LPCTSTR lpszFilePath,SLIDER_TYPE Type);

protected:
	CPCHunter				*m_pMainWnd;
	CDuiString				 m_strXMLPath;
	CAnimationTabLayoutUI *m_pSkinTab;
};


