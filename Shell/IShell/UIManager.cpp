#include "StdAfx.h"
#include "UIManager.h"

namespace IShell
{
	CPaintManagerUI::CPaintManagerUI()
		: m_hWndPaint(NULL)
		, m_hDcPaint(NULL)
		, m_hDcOffscreen(NULL)
		, m_hbmpOffscreen(NULL)
		, m_hbmpBackground(NULL)
		, m_hDcBackground(NULL)
	{
		/*m_pRoot = new CLabelUI;*/
	}

	CPaintManagerUI::~CPaintManagerUI()
	{
		if (m_hDcOffscreen != NULL)		::DeleteDC(m_hDcOffscreen);
		if (m_hDcBackground != NULL) ::DeleteDC(m_hDcBackground);
		if (m_hbmpOffscreen != NULL)	::DeleteObject(m_hbmpOffscreen);
		if (m_hbmpBackground != NULL)	::DeleteObject(m_hbmpBackground);
		if (m_hDcPaint != NULL)		::ReleaseDC(m_hWndPaint, m_hDcPaint);
		/*delete m_pRoot;*/
	}

	void CPaintManagerUI::Init(HWND hWnd)
	{
		ASSERT(::IsWindow(hWnd));

		if (m_hWndPaint != hWnd)
		{
			m_hWndPaint = hWnd;
			m_hDcPaint = ::GetDC(hWnd);
		}
	}

	BOOL CPaintManagerUI::IsLayered() const
	{
		return FALSE;
	}

	BOOL CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		if (m_hWndPaint == NULL) return FALSE;

		return FALSE;
	}
}