#pragma once

class CStaticArpDialog : public WindowImplBase
{
	public:
		CStaticArpDialog(HWND hWnd);
		~CStaticArpDialog();

	public:
		void Notify(TNotifyUI& msg);
		virtual void OnFinalMessage(HWND hWnd);
		virtual void InitWindow();
		virtual CDuiString GetSkinFile();
		virtual CDuiString GetSkinFolder();
		virtual LPCTSTR GetWindowClassName(void) const;
	protected:
		virtual void OnClick(TNotifyUI& msg);
		void OnOK(TNotifyUI& msg);

	protected:
		//virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
};