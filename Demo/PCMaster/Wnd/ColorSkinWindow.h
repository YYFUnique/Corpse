#pragma once
#include "../PCMaster.h"
class CColorSkinWindow : public WindowImplBase
{
public:
	CColorSkinWindow(CPCMaster* pPCMaster, CDuiRect pRect);
	~CColorSkinWindow();
public:
	LPCTSTR GetWindowClassName() const;
	void Notify(TNotifyUI& msg);

	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
protected:
	virtual void OnClick(TNotifyUI& msg);
	void OnValueChanged(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
private:
	CDuiRect		m_lpRectParent;
	CPCMaster* m_pPCMaster;
};