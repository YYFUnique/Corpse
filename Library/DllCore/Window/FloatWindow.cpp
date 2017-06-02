#include "StdAfx.h"
#include "FloatWindow.h"

CEventMainWnd CFloatWindow::m_sHookMainWnd;

CFloatWindow::CFloatWindow()
{
	m_hEventHook = NULL;
}

CFloatWindow::~CFloatWindow()
{
	ReleaseHookWinEvent();
}

BOOL CFloatWindow::StickWndToDesktop(HWND hMainWnd)
{	
	if (m_hEventHook != NULL)
		return FALSE;

	//m_hEventHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE, NULL, WinEventHookProc, 0, 0, WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS);
	m_hEventHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventHookProc, 0, 0, WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS);
	if (m_hEventHook != NULL)
	{
		EventHookWnd HookWnd;
		HookWnd.hMainWnd = hMainWnd;
		HookWnd.hEventHook = m_hEventHook;
		m_sHookMainWnd.AddTail(HookWnd);
	}
	return m_hEventHook != NULL;
}

void CFloatWindow::CannelWndSticked(HWND hMainWnd)
{
	POSITION pos = m_sHookMainWnd.GetHeadPosition();
	while(pos)
	{
		POSITION posOld = pos;
		const EventHookWnd& HookWnd = m_sHookMainWnd.GetNext(pos);
		if (HookWnd.hMainWnd == hMainWnd)
			m_sHookMainWnd.RemoveAt(posOld);
	}
}

void CFloatWindow::ReleaseHookWinEvent()
{
	if (m_hEventHook != NULL)
	{
		UnhookWinEvent(m_hEventHook);
		POSITION pos = m_sHookMainWnd.GetHeadPosition();
		while(pos)
		{
			POSITION posOld = pos;
			const EventHookWnd HookWnd = m_sHookMainWnd.GetNext(pos);
			if (HookWnd.hEventHook == m_hEventHook)
				m_sHookMainWnd.RemoveAt(posOld);
		}
		m_hEventHook = NULL;
	}
}

void CFloatWindow::WinEventHookProc(HWINEVENTHOOK hEventHook, DWORD dwEvent, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	//判断当前窗口是否有效
	if (IsWindow(hWnd))
	{ 
		TCHAR szClassName[MAX_PATH];
		GetClassName(hWnd, szClassName, _countof(szClassName));

		POSITION pos = m_sHookMainWnd.GetHeadPosition();
		HWND hFloatWnd = NULL;
		while(pos)
		{
			const EventHookWnd& HookWnd = m_sHookMainWnd.GetNext(pos);
			if (HookWnd.hEventHook == hEventHook)
			{
				hFloatWnd = HookWnd.hMainWnd;
				break;
			}
		}

		//如果窗口无效，返回
		if (hFloatWnd == NULL || IsWindow(hFloatWnd) == FALSE)
			return;

		if (dwEvent == EVENT_SYSTEM_FOREGROUND)
		{
			if (_tcsicmp(szClassName, _T("WorkerW")) == 0)
			{
				SetWindowPos(hFloatWnd, HWND_TOPMOST,0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			}
			else 
			{
				//判断当前是否处于顶层窗体，如果是将窗体置于底层
				DWORD dwStyle = GetWindowLongPtr(hFloatWnd, GWL_EXSTYLE);
				if (dwStyle & WS_EX_TOPMOST)
					SetWindowPos(hFloatWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			}
		}
	}
}