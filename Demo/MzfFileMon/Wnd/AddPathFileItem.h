#pragma once

#include "DrvControl/FileSystemFilter.h"

class CAddPathFileItem : public WindowImplBase
{
public:
	CAddPathFileItem(HWND hWnd);
	~CAddPathFileItem();
public:
	virtual void InitWindow();
	//void OnFinalMessage(HWND hWnd);
	void Notify(TNotifyUI& msg);

	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetWindowClassName(void) const;
public:
	DWORD GetMatchMode() const;
	CDuiString	GetMatchFilePath() const;
	BOOL	IsMatchRuleEnabled() const;
protected:
	void OnFinish();
	void OnClick(TNotifyUI& msg);
	void OnReturn(TNotifyUI& msg);
	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
protected:
	BOOL			m_bEnabled;
	DWORD		m_dwMatchMode;
	CDuiString	m_strFilePath;
};