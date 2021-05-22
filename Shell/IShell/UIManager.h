#pragma once

namespace IShell
{
	class SHELL_API CPaintManagerUI
	{
	public:
		CPaintManagerUI();
		~CPaintManagerUI();
	public:
		void Init(HWND hWnd);
		BOOL IsLayered() const;
		BOOL MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	protected:
		HWND		m_hWndPaint;
		HDC			m_hDcPaint;
		HDC			m_hDcOffscreen;
		HDC			m_hDcBackground;
		HBITMAP m_hbmpOffscreen;
		HBITMAP m_hbmpBackground;
		/*CLabelUI* m_pRoot;*/
	};
}