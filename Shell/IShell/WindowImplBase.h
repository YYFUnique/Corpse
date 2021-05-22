#pragma once

namespace IShell
{
	class SHELL_API WindowImplBase
	{
	public:
		WindowImplBase();
		virtual ~WindowImplBase();

	public:
		void Init(HWND hWnd);
		virtual void OnFinalMessage(HWND hWnd);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	protected:
		static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		HWND						m_hWnd;
		WNDPROC				m_OldWndProc;
		CPaintManagerUI	m_PaintManager;
	};
}