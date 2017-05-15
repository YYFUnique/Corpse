#include "StdAfx.h"
#include "Resource.h"
#include "xDate.h"
#include "TTimeHelper.h"

#include "DllCore/Utils/Security.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Json/JsonObject.h"
#include "DllCore/Encrypt/Base64.h"
#include "DllCore/Utils/TextTools.h"

#include <dwmapi.h>
#pragma comment(lib, "shlwapi.lib")

#define FRAME_TIMERD_ID				0x1002
#define LAYOUT_HEAD_TIMED_ID		0x1003
//#define WM_QUIT_MSG_LOOP			WM_USER+10   //退出消息循环
const LPCTSTR DayOfWeek[] = {_T("星期天"),_T("星期一"),_T("星期二"),_T("星期三"),_T("星期四"),_T("星期五"),_T("星期六")};
//#define WM_DWMSENDICONICLIVEPREVIEWBITMAP   0x0326

CTTimeHelper::CTTimeHelper()
{
	m_pHLayoutHead = NULL;
	m_bShowChild = FALSE;
	ZeroMemory(&m_SysTime, sizeof(m_SysTime));
	m_SysTime.wHour = -1;
	m_SysTime.wMinute = -1;
	m_SysTime.wSecond = -1;
	m_SysTime.wDay = -1;
	m_SysTime.wMonth = -1;
	m_SysTime.wDayOfWeek = m_wDayOfWeek = -1;
	m_hWnd = NULL;

	m_pFloatWindow = NULL;
	m_pTxMiniSkin = NULL;
	m_pLibcurl = NULL;
	m_pWeatherInfo = NULL;
}

CTTimeHelper::~CTTimeHelper()
{
	//清理桌面漂浮窗口
	if (m_pFloatWindow != NULL)
	{
		delete m_pFloatWindow;
		m_pFloatWindow = NULL;
	}

	if (m_pLibcurl != NULL)
	{
		delete m_pLibcurl;
		m_pLibcurl = NULL;
	}

	if (m_pWeatherInfo != NULL)
	{
		delete m_pWeatherInfo;
		m_pWeatherInfo = NULL;
	}
}

void CTTimeHelper::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CTTimeHelper::GetWindowClassName() const
{
	return _T("TTimeHelper");
}

CDuiString CTTimeHelper::GetSkinFile()
{
	return _T("TTimeHelper.xml");
}

CDuiString CTTimeHelper::GetZIPFileName() const
{
	return _T("TTimeHelper.zip");
}

CDuiString CTTimeHelper::GetSkinFolder()
{
	return _T("TTimeHelper");
}

UILIB_RESOURCETYPE CTTimeHelper::GetResourceType() const
{
	return UILIB_FILE;
}

void CTTimeHelper::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	TCHAR szModuleFile[MAX_PATH];
	GetModuleFileName(NULL,szModuleFile,_countof(szModuleFile));
	PathRemoveFileSpec(szModuleFile);
	PathAppend(szModuleFile,_T("HookWindows.dll"));
	
	m_pFloatWindow = new CFloatWindow;
	m_pFloatWindow->SetWndStickToDesktop(m_hWnd);
	
	HMODULE hModule = LoadLibrary(_T("dwmapi.dll"));
	if (hModule != NULL)
	{
		typedef HRESULT (WINAPI* DwmSetWindowAttributeFn)(HWND, DWORD, LPCVOID, DWORD);
		DwmSetWindowAttributeFn pFnDwmSetWindowAttribute = (DwmSetWindowAttributeFn)GetProcAddress(hModule, "DwmSetWindowAttribute");
		if (pFnDwmSetWindowAttribute != NULL)
		{
			DWMNCRENDERINGPOLICY RenderPolicy = DWMNCRP_ENABLED;

			//由于枚举值DWMWA_EXCLUDED_FROM_PEEK只存在于v7.0的SDK中，v6aSDK不存在，故直接使用其值
			//DwmSetWindowAttribute(m_hWnd, 12, &renderPolicy, sizeof(int));
			pFnDwmSetWindowAttribute(m_hWnd, 12, &RenderPolicy, sizeof(int));
		}
		FreeLibrary(hModule);
	}

	/*CLabelUI* pText = (CLabelUI*)m_PaintManager.FindControl(_T("News"));
	UINT nUnicode[] = {0x06DE};
	pText->SetText((LPCTSTR)nUnicode);
*/
	//设置窗口显示位置
	TCHAR szTimeMgr[MAX_PATH];
	GetModuleFileName(NULL, szTimeMgr, _countof(szTimeMgr));
	PathRenameExtension(szTimeMgr, _T(".ini"));

	POINT ptClientPos = {0};
	if (PathFileExists(szTimeMgr))
	{
		ptClientPos.x = GetPrivateProfileInt(_T("Position"), _T("left"), 0, szTimeMgr);
		ptClientPos.y = GetPrivateProfileInt(_T("Position"), _T("top"), 0, szTimeMgr);
	}

	RECT rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	if (ptClientPos.x == 0 && ptClientPos.y ==0)
		CenterWindow();
	else
		MoveWindow(m_hWnd, ptClientPos.x, ptClientPos.y, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, FALSE);

	//初始化Head控件指针
	m_pHLayoutHead = m_PaintManager.FindControl(_T("HLayoutHead"));
	SetChildVisible(m_pHLayoutHead, FALSE);
	int i=0;
	CContainerUI* pParent = (CContainerUI*)m_PaintManager.FindControl(_T("HLayoutMain"));
	for (int n=0; n<pParent->GetCount(); ++n)
	{
		CControlUI* pControl = pParent->GetItemAt(n);
		CFrameSeqUI* pFrame = (CFrameSeqUI*)pControl->GetInterface(DUI_CTR_FRAMESEQ);
		if (pFrame != NULL)
			m_pFrame[i++] = pFrame;
	}
}

CControlUI* CTTimeHelper::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pControl = NULL;
	ASSERT(pControl && pstrClass);
	return pControl;
}

void CTTimeHelper::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_TIMER)
		OnTimer(msg);
	else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnInitDialog();
}

void CTTimeHelper::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
	{
		//由于Duilib窗口类，在窗口关闭时，会自动调用OnFinalMessage()，此时将自动delete对象
		//必须调用关闭窗口才会触发，如果直接PostQuitMessage则不会触发
		if (m_pTxMiniSkin == NULL)
		{
			m_pTxMiniSkin = new CTxMiniSkin(m_hWnd);

			UINT nRet = m_pTxMiniSkin->ShowModal();

			if (nRet == IDOK)
			{
				Close(IDOK);
				PostQuitMessage(0);
			}
		}
	}
	else if (msg.pSender == m_PaintManager.FindControl(_T("News")))
	{
		ShellExecute(m_hWnd, _T("open"), _T("http://www.qq.com/"), NULL, NULL, SW_SHOW);
	}
}

void CTTimeHelper::OnTimer(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("HLayoutMain")))
	{
		if (msg.wParam == FRAME_TIMERD_ID)
			SetShowTimer();
	}
	else if (msg.pSender == m_PaintManager.FindControl(_T("HLayoutHead")))
	{
		if (msg.wParam == LAYOUT_HEAD_TIMED_ID)
		{
			if (PtInRect() == FALSE && m_bShowChild == TRUE && m_pHLayoutHead)
			{
				SetChildVisible(m_pHLayoutHead, FALSE);
				m_PaintManager.KillTimer(m_pHLayoutHead);
			}
		}
	}
}

void CTTimeHelper::OnInitDialog()
{
	//SetForegroundWindow(m_hWnd);
	SetShowTimer();

	CControlUI* pControl = m_PaintManager.FindControl(_T("HLayoutMain"));
	m_PaintManager.SetTimer(pControl, FRAME_TIMERD_ID, 1000);

	LPCTSTR lpszAk = _T("tx0s5wZjHvEunl28nOu1a2s4HbRfZvy6");
	LPCTSTR lpszBaiDuAPI = _T("http://api.map.baidu.com/location/ip?ak=%s");
	CDuiString strWeatherInfo;
	strWeatherInfo.Format(lpszBaiDuAPI, lpszAk);
	//strWeatherInfo.Format(lpszBaiDuAPI, _T("成都"), lpszAk);

	//CString strCryptURL;
	//URLEncode(strWeatherInfo, strCryptURL);

	/*TCHAR szBase64Code[1024*2];
	int nSize = _countof(szBase64Code);
	Base64Encode((LPCTSTR)strWeatherInfo, strWeatherInfo.GetLength(), szBase64Code, &nSize, ATL_BASE64_FLAG_NOCRLF);
	*/
	if (m_pWeatherInfo == NULL)
	{
		m_pWeatherInfo = new CWeatherHelper;

	}
	/*if (m_pLibcurl == NULL)
	{
		
	}*/
}

void CTTimeHelper::SetShowTimer()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	CDuiString strTime;

	DWORD dwTime, dwPreTime, dwNowTime;

	/*if (m_SysTime.wSecond != SysTime.wSecond)
	{
		dwTime = SysTime.wSecond%10 - 1;
		if (dwTime == -1)
			dwTime = 9;
		dwPreTime = dwTime%10;
		dwNowTime = ++dwTime%10;
		strTime.Format(_T("%d_%d.png"), dwPreTime, dwNowTime);

		m_pFrame[3]->SetFrameImage(strTime);
		m_pFrame[3]->Run();

		if (m_SysTime.wSecond/10 != SysTime.wSecond/10)
		{
			DWORD dwTime = SysTime.wSecond/10 - 1;
			if (dwTime == -1)
				dwTime = 5;
			DWORD dwNow = dwTime%6;
			DWORD dwNext = ++dwTime%6;
			strTime.Format(_T("%d_%d.png"), dwNow, dwNext);

			m_pFrame[2]->SetFrameImage(strTime);
			m_pFrame[2]->Run();
		}

		m_SysTime.wSecond = SysTime.wSecond;
	}
*/
	if (m_SysTime.wMinute != SysTime.wMinute)
	{
		dwTime = SysTime.wMinute%10 - 1;
		if (dwTime == -1)
			dwTime = 9;
		dwPreTime = dwTime%10;
		dwNowTime = ++dwTime%10;
		strTime.Format(_T("%d_%d.png"), dwPreTime, dwNowTime);

		m_pFrame[3]->SetFrameImage(strTime);
		m_pFrame[3]->Run();

		if (m_SysTime.wMinute/10 != SysTime.wMinute/10)
		{
			dwTime = SysTime.wMinute/10 - 1;
			if (dwTime == -1)
				dwTime = 5;

			dwPreTime = dwTime%6;
			dwNowTime = ++dwTime%6;
			strTime.Format(_T("%d_%d.png"), dwPreTime, dwNowTime);

			m_pFrame[2]->SetFrameImage(strTime);
			m_pFrame[2]->Run();
		}

		m_SysTime.wMinute = SysTime.wMinute;
	}

	if (m_SysTime.wHour != SysTime.wHour)
	{
		DWORD dwTenKey = SysTime.wHour/10;				//判断小时的十位上是否是2
		DWORD dwMod = 10;
		dwTime = SysTime.wHour%10 - 1;
		if (dwTenKey == 2)
		{
			if (dwTime == -1)
				dwTime = 3;
			dwMod = 4;
		}
		else
		{
			if (dwTime == -1)
				dwTime = 9;
		}
		
		dwPreTime = dwTime%10;
		dwNowTime = ++dwTime % dwMod;
		
		strTime.Format(_T("%d_%d.png"), dwPreTime, dwNowTime);

		m_pFrame[1]->SetFrameImage(strTime);
		m_pFrame[1]->Run();

		if (m_SysTime.wHour/10 != SysTime.wHour/10)
		{
			dwTime = SysTime.wHour/10 - 1;
			if (dwTime == -1)
				dwTime = 2;

			dwPreTime = dwTime%3;
			dwNowTime = ++dwTime%3;
			strTime.Format(_T("%d_%d.png"), dwPreTime, dwNowTime);

			m_pFrame[0]->SetFrameImage(strTime);
			m_pFrame[0]->Run();
		}

		m_SysTime.wHour = SysTime.wHour;
	}

	//显示国历
	if (m_SysTime.wMonth != SysTime.wMonth || m_SysTime.wDay != SysTime.wDay)
	{
		CLabelUI* pDate = (CLabelUI*)m_PaintManager.FindControl(_T("Date"));
		if (pDate)
		{
			CDuiString strDate;
			strDate.Format(_T("%d月%d日"), SysTime.wMonth, SysTime.wDay);
			pDate->SetText(strDate);
		}

		//显示阴历
		CLabelUI* pLunarDay = (CLabelUI*)m_PaintManager.FindControl(_T("LunarDay"));
		if (pLunarDay)
		{
			WORD wYear,wMonth,wDay;
			GetLunarDate(SysTime.wYear, SysTime.wMonth, SysTime.wDay, wYear, wMonth, wDay);

			TCHAR szLunarMonth[10] = {0};
			FormatMonth(wMonth, szLunarMonth, TRUE);
			TCHAR szLunarDay[10] = {0};
			FormatLunarDay(wDay, szLunarDay);

			CDuiString strLunarTime;
			strLunarTime.Format(_T("%s%s"), szLunarMonth, szLunarDay);

			pLunarDay->SetText(strLunarTime);
		}

		m_SysTime.wMonth = SysTime.wMonth;
		m_SysTime.wDay = SysTime.wDay;
	}

	//显示星期
	if (m_wDayOfWeek != SysTime.wDayOfWeek)
	{
		CLabelUI* pDayOfWeek = (CLabelUI*)m_PaintManager.FindControl(_T("DayOfWeek"));
		if (pDayOfWeek)
			pDayOfWeek->SetText(DayOfWeek[SysTime.wDayOfWeek]);

		m_wDayOfWeek = SysTime.wDayOfWeek;
	}
}

BOOL CTTimeHelper::PtInRect()
{
	POINT pt;
	GetCursorPos(&pt);
	::ScreenToClient(*this, &pt);

	CControlUI* pMainControl = m_PaintManager.FindControl(_T("VLayoutMain"));
	if (pMainControl == NULL)
		return FALSE;

	RECT rcPos = pMainControl->GetPos();
	return ::PtInRect(&rcPos, pt);
}

LRESULT CTTimeHelper::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (PtInRect() == FALSE && m_bShowChild == TRUE && m_pHLayoutHead)
	{
		SetChildVisible(m_pHLayoutHead, FALSE);
		m_PaintManager.KillTimer(m_pHLayoutHead);
	}

	return WindowImplBase::OnKillFocus(uMsg, wParam, lParam, bHandled);
}

LRESULT CTTimeHelper::ResponseDefaultKeyEvent(WPARAM wParam)
{
	return FALSE;
}

LRESULT CTTimeHelper::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (PtInRect() && m_bShowChild == FALSE && m_pHLayoutHead)
	{
		SetChildVisible(m_pHLayoutHead, TRUE);
		m_PaintManager.SetTimer(m_pHLayoutHead, LAYOUT_HEAD_TIMED_ID, 300);
	}
	
	return WindowImplBase::OnNcHitTest(uMsg, wParam, lParam ,bHandled);
}

BOOL CTTimeHelper::SetChildVisible(CControlUI* pControl, bool bVisible)
{
	if (pControl == NULL)
		return FALSE;

	CContainerUI* pLayout = (CContainerUI*)pControl->GetInterface(DUI_CTR_CONTAINER);
	ASSERT(pLayout);
	if (pLayout == NULL)
		return FALSE;

	for (int n=0; n<pLayout->GetCount(); ++n)
	{
		CControlUI* pChild = pLayout->GetItemAt(n);
		if (pChild && pChild->IsVisible() != bVisible)
			pChild->SetVisible(bVisible);
	}

	m_bShowChild = bVisible;

	return TRUE;
}

HRESULT CTTimeHelper::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(uMsg)
	{
		case WM_WINDOWPOSCHANGED:
			{
				WINDOWPOS* pWinPos = (WINDOWPOS*)lParam;
				if (pWinPos->flags & SWP_NOMOVE)
					break;

				POINT ptClientPos = {pWinPos->x, pWinPos->y};

				if (ptClientPos.x == 0 && ptClientPos.y == 0)
					break;

				TCHAR szModuleFile[MAX_PATH];
				GetModuleFileName(NULL, szModuleFile, _countof(szModuleFile));
				PathRenameExtension(szModuleFile, _T(".ini"));

				TCHAR szClientPos[10];
				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), ptClientPos.x);
				WritePrivateProfileString(_T("POSITION"), _T("left"), szClientPos, szModuleFile);

				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), ptClientPos.y);
				WritePrivateProfileString(_T("POSITION"), _T("top"), szClientPos, szModuleFile);
			}
			break;
	}

	return FALSE;
}


