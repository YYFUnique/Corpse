#include "StdAfx.h"
#include "Platform.h"

Window::Window()
	: m_wid(NULL)
	, m_cursorLast(cursorInvalid)
{

}

Window::Window(const Window& winSource)
	: m_wid(winSource.m_wid)
	, m_cursorLast(winSource.m_cursorLast)
{

}

Window::~Window()
{

}

Window& Window::operator=(WindowID wid)
{
	m_wid = wid;
	return *this;
}

WindowID Window::GetID() const
{
	return m_wid;
}

BOOL Window::IsCreated() const
{
	return m_wid != NULL;
}

void Window::Destroy()
{
	if (m_wid)
		::DestroyWindow(reinterpret_cast<HWND>(m_wid));

	m_wid = NULL;
}

BOOL Window::HasFocus() const
{
	return GetFocus() == m_wid;
}

CDuiRect Window::GetPosition() const
{
	RECT rcWnd;
	GetWindowRect(reinterpret_cast<HWND>(m_wid), &rcWnd);
	return CDuiRect(rcWnd);
}

void Window::SetPosition(const CDuiRect* prcPos)
{
	SetWindowPos(reinterpret_cast<HWND>(m_wid), 0, prcPos->left, prcPos->top, prcPos->GetWidth(), prcPos->GetHeight(), SWP_NOZORDER|SWP_NOACTIVATE);
}

void Window::SetPositionRelative(const CDuiRect* prcPos, Window relativeTo)
{

}

void Window::Show(BOOL bSHow /*= TRUE*/)
{

}

void Window::SetFont(Font &font)
{

}