#include "StdAfx.h"
#include "Resource.h"
#include "TTimeHelper.h"
#include "Lunar.h"
#include "CityWeatherInfo.h"

#include "DllCore/Utils/Security.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Utils/Registry.h"
#include "DllCore/Json/JsonObject.h"
#include "DllCore/Encrypt/Base64.h"
#include "DllCore/Utils/TextTools.h"

#include <atltime.h>
#pragma comment(lib, "shlwapi.lib")

#define FRAME_TIMERD_ID						0x1002		//刷新时间的定时器ID
#define LAYOUT_HEAD_TIMED_ID				0x1003		//检测是否显示标题的定时器ID
#define WEATHER_SHOW_TIMERD_ID		0x1004		//程序启动后，获取天气信息ID
#define WEATHER_UPDATE_TIMERD_ID	0x1005		//定时更新天气ID
#define WEATHER_API_TIMEOUT_TIMER_ID	0x1006		//	天气接口到期消息

//#define WM_QUIT_MSG_LOOP			WM_USER+10   //退出消息循环
const LPCTSTR DayOfWeek[] = {_T("星期天"),_T("星期一"),_T("星期二"),_T("星期三"),_T("星期四"),_T("星期五"),_T("星期六")};
//#define WM_DWMSENDICONICLIVEPREVIEWBITMAP   0x0326

#define INI_APP_NAME	_T("mail")		//	配置文件中app名称
// 
// const UINT WM_TOGGLEDESKTOP			= RegisterWindowMessage(TOGGLEDESKTOP);

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
	m_SysTime.wDayOfWeek = -1;
	m_hWnd = NULL;

	m_pFloatWindow = NULL;
	m_pTxMiniSkin = NULL;
	m_pWeatherInfo = NULL;
	m_pCityInfo = NULL;

	HWND hTxMini = FindWindow(_T("MiniSkin"), NULL);
	Create(hTxMini, _T("TimerHelper"), WS_POPUP, WS_EX_TOOLWINDOW, 0, 0, 0, 0, NULL);
}

CTTimeHelper::~CTTimeHelper()
{
	//清理桌面漂浮窗口
	if (m_pFloatWindow != NULL)
	{
		delete m_pFloatWindow;
		m_pFloatWindow = NULL;
	}

	if (m_pCityInfo != NULL)
	{
		delete m_pCityInfo;
		m_pCityInfo = NULL;
	}

	if (m_pWeatherInfo != NULL)
	{
		delete m_pWeatherInfo;
		m_pWeatherInfo = NULL;
	}

	//	无需单独释放线程对象，由框架自动释放
	// m_pSendMailThread;
	//CLsThreadMgr& LsThreadMgr = GetLsThreadMgr();
	//LsThreadMgr.DeleteAllThread();
}

void CTTimeHelper::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	//delete this;
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
#ifdef _DEBUG
	return _T("TTimeHelper");
#else
	return _T("");
#endif
}

UILIB_RESOURCETYPE CTTimeHelper::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

void CTTimeHelper::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	//////////////////////////////////////////////////////////////////////////
	// 嵌入桌面，成为桌面的一个子窗口，不受Win + D影响
	// 桌面进程挂掉后，窗口不能继续显示，但是进程可以捕捉任务栏窗口创建消息
	// 从而继续设置父窗口
	/*HWND hProgram = FindWindow(_T("Progman"), _T("Program Manager"));
	SetParent(m_hWnd, hProgram);
	BringWindowToTop(m_hWnd);*/

	// 使用系统HOOK方式，由系统通知应用特定窗口状态变化
	//m_pFloatWindow = new CFloatWindow;
	//m_pFloatWindow->StickWndToDesktop(m_hWnd);
	
	DwmHelper* pDwmHelper = DwmHelper::GetInstance();
	if (pDwmHelper != NULL)
	{
		// Areo模式中，可以预览应用
		pDwmHelper->SetAreoWithPeek(m_hWnd);
		// 3D切换时，应用显示在上层
		pDwmHelper->SetFlip3DPolicy(m_hWnd);
	}

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
		CFrameUI* pFrame = (CFrameUI*)pControl->GetInterface(DUI_CTR_FRAME);
		if (pFrame != NULL)
			m_pFrame[i++] = pFrame;
	}

	/*if (m_pMailHelper == NULL)
	{
		m_pMailHelper = new CMailHelper;

		m_pSendMailThread = new CSendMailThread;
		CLsThreadMgr& LsThreadMgr = GetLsThreadMgr();
		m_pSendMailThread->SetMailHelper(m_pMailHelper);

		LsThreadMgr.AddThreadToList(m_pSendMailThread);
		m_pSendMailThread->StartThread();

		PraseMailInfo();
	}*/
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

BOOL CTTimeHelper::Receive(CityInfoChangedParam CityInfoParam)
{
	if (CityInfoParam.CityInfoChanged == CITYINFO_CHANGED_LOAD_SUCCESS)
	{
		if (m_pWeatherInfo)
			m_pWeatherInfo->GetCityLocation();
	}
	else if (CityInfoParam.CityInfoChanged == CITYINFO_CHANGED_LOCATION)
	{
		//保存城市位置信息
		m_strCityLocation = CityInfoParam.strJsonString;
		//如果位置信息发生变更，则重新获取天气和城市控制质量
		if (m_pWeatherInfo)
		{
			m_pWeatherInfo->GetCityWeather(CityInfoParam.strJsonString);
			m_pWeatherInfo->GetCityAQIInfo(CityInfoParam.strJsonString);
		}
	}
	else if (CityInfoParam.CityInfoChanged == CITYINFO_CHANGED_TEMPERATURE)
	{
		//城市天气信息获取成功
		CButtonUI* pWeather = (CButtonUI*)m_PaintManager.FindControl(_T("Temperature"));
		if (pWeather)
			pWeather->SetText(CityInfoParam.strJsonString);
	}

	return TRUE;
}

void CTTimeHelper::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
	{
		// 由于Duilib窗口类，在窗口关闭时，会自动调用OnFinalMessage()，此时将自动delete对象
		// 必须调用关闭窗口才会触发，如果直接PostQuitMessage则不会触发
		m_pTxMiniSkin = new CTxMiniSkin(m_hWnd);

		UINT nRet = m_pTxMiniSkin->ShowModal();

		if (nRet == IDOK)
		{
			Close(IDOK);
			// 通知父窗口关闭，以减少插件引用次数
			HWND hParent = GetParent(m_hWnd);
			if (hParent != NULL)
				::SendMessage(hParent, WM_CLOSE, IDOK ,NULL);
			PostQuitMessage(0);
		}
	}
	else if (msg.pSender == m_PaintManager.FindControl(_T("News")))
	{
		ShellExecute(m_hWnd, _T("open"), _T("http://www.qq.com/"), NULL, NULL, SW_SHOW);
	}
	else if (msg.pSender == m_PaintManager.FindControl(_T("Temperature")))
	{
		if (m_pWeatherInfo != NULL)
		{
			if (m_strCityLocation.IsEmpty() != FALSE)
				m_pWeatherInfo->GetCityLocation();
			else
				m_pWeatherInfo->GetCityWeather(m_strCityLocation);
		}
	}
}

void CTTimeHelper::OnTimer(TNotifyUI& msg)
{
	if (msg.wParam == FRAME_TIMERD_ID)
		SetShowTimer();
	else	if (msg.wParam == LAYOUT_HEAD_TIMED_ID)
	{
		if (PtInRect(_T("VLayoutMain")) == FALSE && m_bShowChild == TRUE && m_pHLayoutHead)
		{
			SetChildVisible(m_pHLayoutHead, FALSE);
			m_PaintManager.KillTimer(m_pHLayoutHead);
		}
	}
	else if (msg.wParam == WEATHER_SHOW_TIMERD_ID)
	{
		if (m_pWeatherInfo == NULL)
		{
			m_pWeatherInfo = new CWeatherHelper;
			m_pWeatherInfo->Init(m_pCityInfo);
		}
		m_pCityInfo->LoadData();

		CControlUI* pWeatherInfo = m_PaintManager.FindControl(_T("Temperature"));
		if (pWeatherInfo)
		{
			m_PaintManager.KillTimer(pWeatherInfo);
			//以后每10分钟刷新一次
			m_PaintManager.SetTimer(pWeatherInfo, WEATHER_UPDATE_TIMERD_ID, 10*60*1000);
		}
	}
	else if (msg.wParam == WEATHER_UPDATE_TIMERD_ID)
	{
		if (m_pWeatherInfo != NULL)
		{
			//如果城市信息为空，那么重新获取城市信息
			if (m_strCityLocation.IsEmpty() != FALSE)
				m_pWeatherInfo->GetCityLocation();
			else
			{
				m_pWeatherInfo->GetCityWeather(m_strCityLocation);
				m_pWeatherInfo->GetCityAQIInfo(m_strCityLocation);
			}
		}
	}
	else if (msg.wParam == WEATHER_API_TIMEOUT_TIMER_ID)
	{
		//if (m_pMailHelper == NULL)
		//	return;

		//CTime Time = CTime::GetCurrentTime();

		//if (Time.GetTime() > m_dwOutOfTime)
		//{
		//	DWORD dwSend = FALSE;
		//	CString strRegPath = GetRegistryPath(_T("TTimeHelper"));
		//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("bSendMail"), dwSend);
		//	//	判断是否发送过邮件，如果已经发送过了，无需重复发送
		//	if (dwSend != FALSE)
		//		m_PaintManager.KillTimer(msg.pSender,msg.wParam);

		//	if (dwSend == FALSE)
		//	{
		//		if (m_pSendMailThread != NULL)
		//			m_pSendMailThread->SetNotificationEvent();
		//	}
		//}
	}
}

void CTTimeHelper::OnInitDialog()
{
	SetShowTimer();

	CControlUI* pControl = m_PaintManager.FindControl(_T("HLayoutMain"));
	m_PaintManager.SetTimer(pControl, FRAME_TIMERD_ID, 1000);

	if (m_pCityInfo == NULL)
	{
		m_pCityInfo = new CCityHelper;
		m_pCityInfo->Init(this);
	}

	CControlUI* pWeatherInfo = m_PaintManager.FindControl(_T("Temperature"));
	if (pWeatherInfo)
		m_PaintManager.SetTimer(pWeatherInfo, WEATHER_SHOW_TIMERD_ID, 1000);

}

void CTTimeHelper::PraseMailInfo()
{
	DWORD dwSend = 0;
	CString strRegPath = GetRegistryPath(_T("TTimeHelper"));
	if (LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("bSendMail"), dwSend) == FALSE)
		return;

	if (dwSend != FALSE)
	{
		//delete m_pMailHelper;
		//m_pMailHelper = NULL;
		return;
	}

	//if (m_pMailHelper != NULL)
	//{
	//	CString strMailInfo;

	//	CString strName;
	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("ServerName"), strName);
	//	m_pMailHelper->SetServerName(strName);

	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("SenderName"), strName);
	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("SenderAddr"), strMailInfo);
	//	m_pMailHelper->SetSenderInfo(strName, strMailInfo);

	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("UserName"), strName);
	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("UsePwd"), strMailInfo);
	//	m_pMailHelper->MailSenderLogin(strName, strMailInfo);

	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("ReceiverName"), strName);
	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("ReceiverAddr"), strMailInfo);
	//	m_pMailHelper->SetReceiverInfo(strName, strMailInfo);

	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("Subject"), strMailInfo);
	//	m_pMailHelper->SetSubject(strMailInfo);

	//	MAIL_CONTENT_TYPE MailType = MAIL_CONTENT_TYPE_TEXT;
	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("Type"), strMailInfo);
	//	if (strMailInfo.CompareNoCase(_T("html")) == 0)
	//		MailType = MAIL_CONTENT_TYPE_HTML;
	//	else if (strMailInfo.CompareNoCase(_T("stream")) == 0)
	//		MailType = MAIL_CONTENT_TYPE_STREAM;

	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("Content"), strMailInfo);
	//	m_pMailHelper->SetMailContent(MailType, strMailInfo);

	//	DWORD dwYear,dwMonth,dwDay;
	//	LsRegQueryValue(HKEY_LOCAL_MACHINE, strRegPath, _T("OutOfTime"), strMailInfo);
	//	_stscanf_s(strMailInfo, _T("%d-%d-%d"),&dwYear,&dwMonth,&dwDay);

	//	CTime Time(dwYear,dwMonth,dwDay,0,0,0);
	//	m_dwOutOfTime = (DWORD)Time.GetTime();

	//	//	设置定时器
	//	CControlUI* pLayout = m_PaintManager.FindControl(_T("VLayoutTotal"));
	//	if (pLayout)
	//	{
	//		m_PaintManager.SetTimer(pLayout, WEATHER_API_TIMEOUT_TIMER_ID, 30*60*1000);
	//		//	模拟触发定时器
	//		TNotifyUI msg;
	//		msg.pSender = pLayout;
	//		msg.wParam = WEATHER_API_TIMEOUT_TIMER_ID;
	//		OnTimer(msg);
	//	}
	//}
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
			BOOL bLeapMonth = 0;
			CLunar Lunar;
			Lunar.GetLunarDayInfo(SysTime.wYear, SysTime.wMonth, SysTime.wDay, wYear, wMonth, wDay, bLeapMonth);

			TCHAR szMonth[] = _T("正二三四五六七八九十冬腊");

			TCHAR szLunarDay[10] = {0};
			CLunar::FormatLunarDay(wDay, szLunarDay);

			CDuiString strLunarTime;
			strLunarTime.Format(_T("%c月%s"), szMonth[wMonth-1], szLunarDay);

			pLunarDay->SetText(strLunarTime);
		}

		m_SysTime.wMonth = SysTime.wMonth;
		m_SysTime.wDay = SysTime.wDay;
	}

	//显示星期
	if (m_SysTime.wDayOfWeek != SysTime.wDayOfWeek)
	{
		CLabelUI* pDayOfWeek = (CLabelUI*)m_PaintManager.FindControl(_T("DayOfWeek"));
		if (pDayOfWeek)
			//设置星期
			pDayOfWeek->SetText(DayOfWeek[SysTime.wDayOfWeek]);

		m_SysTime.wDayOfWeek = SysTime.wDayOfWeek;
	}
}

BOOL CTTimeHelper::PtInRect(LPCTSTR lpszName)
{
	POINT pt;
	GetCursorPos(&pt);
	::ScreenToClient(*this, &pt);

	CControlUI* pMainControl = m_PaintManager.FindControl(lpszName);
	if (pMainControl == NULL)
		return FALSE;

	RECT rcPos = pMainControl->GetPos();
	return ::PtInRect(&rcPos, pt);
}

LRESULT CTTimeHelper::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    CControlUI* pHover = m_PaintManager.FindControl(pt);
    if (pHover == NULL)
		return 0;
    
	//如果没有获取到城市天气信息，不开启城市天气详细信息
    if (pHover->GetName() == _T("Temperature"))
    {
		CDuiString strCityTempture = pHover->GetText();
		if (strCityTempture.CompareNoCase(_T("未知")) == 0)
			return 0;

		HWND hSubWnd = FindWindow(_T("CCityWeatherInfo"),NULL);
		if (::IsWindow(hSubWnd))
			return 0;

		CCityWeatherInfo* pCityWeather = new CCityWeatherInfo(m_hWnd, m_pCityInfo);
		if (pCityWeather != NULL)
			pCityWeather->ShowWindow(true);
	}
    return 0;
}

LRESULT CTTimeHelper::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (PtInRect(_T("VLayoutMain")) == FALSE && m_bShowChild == TRUE && m_pHLayoutHead)
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
	if (PtInRect(_T("VLayoutMain")) && m_bShowChild == FALSE && m_pHLayoutHead)
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

				HWND hOwner = FindWindow(_T("MiniSkin"), NULL);
				if (IsWindow(hOwner))
					SetWindowPos(hOwner, NULL, ptClientPos.x, ptClientPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

				if (ptClientPos.x == 0 && ptClientPos.y == 0)
					break;

				TCHAR szModuleFile[MAX_PATH];
				GetModuleFileName(NULL, szModuleFile, _countof(szModuleFile));
				PathRenameExtension(szModuleFile, _T(".ini"));

				//将程序在屏幕上下左右的位置写入配置文件中
				//左边距离屏幕的距离
				TCHAR szClientPos[10];
				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), ptClientPos.x);
				WritePrivateProfileString(_T("POSITION"), _T("left"), szClientPos, szModuleFile);

				//上边距离屏幕的距离
				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), ptClientPos.y);
				WritePrivateProfileString(_T("POSITION"), _T("top"), szClientPos, szModuleFile);

				RECT rcWnd;
				GetWindowRect(m_hWnd,&rcWnd);

				//右边距离屏幕的距离
				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), GetSystemMetrics(SM_CXSCREEN) - (ptClientPos.x + rcWnd.right - rcWnd.left));
				WritePrivateProfileString(_T("POSITION"), _T("right"), szClientPos, szModuleFile);
				//下边距离屏幕的距离
				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), GetSystemMetrics(SM_CYSCREEN) - (ptClientPos.y + rcWnd.bottom - rcWnd.top));
				WritePrivateProfileString(_T("POSITION"), _T("bottom"), szClientPos, szModuleFile);
			}
			break;
		case WM_DISPLAYCHANGE:
			{
				DWORD dwWidth = LOWORD(lParam);
				DWORD dwHegiht = HIWORD(lParam);

				TCHAR szModuleFile[MAX_PATH];
				GetModuleFileName(NULL, szModuleFile, _countof(szModuleFile));
				PathRenameExtension(szModuleFile, _T(".ini"));

				int nLeft = GetPrivateProfileInt(_T("POSITION"), _T("left"), 0, szModuleFile);
				int nTop = GetPrivateProfileInt(_T("POSITION"), _T("top"), 0, szModuleFile);
				int nRight = GetPrivateProfileInt(_T("POSITION"), _T("right"), 0, szModuleFile);
				int nBottom = GetPrivateProfileInt(_T("POSITION"), _T("bottom"), 0, szModuleFile);

				RECT rcWnd;
				GetWindowRect(m_hWnd,&rcWnd);

				if (nRight < nLeft)	//左边距离屏幕的距离大于右边距离屏幕的距离
					nLeft = dwWidth - ( rcWnd.right - rcWnd.left ) - nRight;

				if (nTop > nBottom)	//下边距离屏幕的距离大于上边距离屏幕的距离
					nTop = dwHegiht - (rcWnd.bottom - rcWnd.top) - nBottom;

				SetWindowPos(m_hWnd, NULL, nLeft, nTop, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			}
			break;
	}

	return FALSE;
}