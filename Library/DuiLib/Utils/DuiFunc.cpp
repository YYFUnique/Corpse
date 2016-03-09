#include "stdafx.h"
#include "DuiFunc.h"

namespace DuiLib
{
	CDuiLibMgr& GetDuiLibMgr()
	{
		static CDuiLibMgr DuiLibInfo;
		static BOOL bInitObject = TRUE;
		if (bInitObject != FALSE)
		{
			//进行一些全局初始化
			bInitObject = FALSE;
		}

		return DuiLibInfo;
	}

	CWindowWnd* GetMainApp()
	{
		return GetDuiLibMgr().GetMainApp();
	}

	CDuiLibMgr::CDuiLibMgr()
		: m_pMainWnd(NULL),
		m_hMainWnd(NULL)
	{

	}

	void CDuiLibMgr::SetMainApp(CWindowWnd* pMainWnd)
	{
		m_pMainWnd = pMainWnd;
	}

	CWindowWnd* CDuiLibMgr::GetMainApp()
	{
		return m_pMainWnd;
	}

	void CDuiLibMgr::SetMainWndHandle(HWND hMainWnd)
	{
		m_hMainWnd = hMainWnd;
	}

	HWND CDuiLibMgr::GetMainWndHandle()
	{
		return m_hMainWnd;
	}
}