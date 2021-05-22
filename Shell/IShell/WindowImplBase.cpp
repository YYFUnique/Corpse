#include "StdAfx.h"
#include "WindowImplBase.h"

namespace IShell
{
	WindowImplBase::WindowImplBase()
	{

	}

	WindowImplBase::~WindowImplBase()
	{

	}

	void WindowImplBase::Init(HWND hWnd)
	{
		m_PaintManager.Init(hWnd);
		m_OldWndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
		if (m_OldWndProc != NULL)
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&__WndProc);

		m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
	}

	void WindowImplBase::OnFinalMessage(HWND hWnd)
	{
		if (m_OldWndProc != 0)
			SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_OldWndProc);
	}

	LRESULT WindowImplBase::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowImplBase* pThis = NULL;
		if( uMsg == WM_NCCREATE ) {
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			pThis = static_cast<WindowImplBase*>(lpcs->lpCreateParams);
			pThis->m_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
		} 
		else {
			pThis = reinterpret_cast<WindowImplBase*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if( uMsg == WM_NCDESTROY && pThis != NULL ) {
				LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
				::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
				/*if( pThis->m_bSubclassed ) pThis->Unsubclass();*/
				pThis->m_hWnd = NULL;
				pThis->OnFinalMessage(hWnd);
				return lRes;
			}
		}
		if( pThis != NULL ) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		} 
		else {
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	LRESULT WindowImplBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;

		lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled) return lRes;

		if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
			return lRes;
		else
			return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
	}

	LRESULT WindowImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return FALSE;
	}
}