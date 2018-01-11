#include "StdAfx.h"
#include "Resource.h"
#include "TTimeHelper.h"
#include "Lunar.h"
#include "CityWeatherInfo.h"

#include "DllCore/Utils/Security.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Json/JsonObject.h"
#include "DllCore/Encrypt/Base64.h"
#include "DllCore/Utils/TextTools.h"

#include <dwmapi.h>
#pragma comment(lib, "shlwapi.lib")

#define FRAME_TIMERD_ID						0x1002		//ˢ��ʱ��Ķ�ʱ��ID
#define LAYOUT_HEAD_TIMED_ID				0x1003		//����Ƿ���ʾ����Ķ�ʱ��ID
#define WEATHER_SHOW_TIMERD_ID		0x1004		//���������󣬻�ȡ������ϢID
#define WEATHER_UPDATE_TIMERD_ID	0x1005		//��ʱ��������ID

//#define WM_QUIT_MSG_LOOP			WM_USER+10   //�˳���Ϣѭ��
const LPCTSTR DayOfWeek[] = {_T("������"),_T("����һ"),_T("���ڶ�"),_T("������"),_T("������"),_T("������"),_T("������")};
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
	m_SysTime.wDayOfWeek = -1;
	m_hWnd = NULL;

	m_pFloatWindow = NULL;
	m_pTxMiniSkin = NULL;
	m_pWeatherInfo = NULL;
	m_pCityInfo = NULL;
}

CTTimeHelper::~CTTimeHelper()
{
	//��������Ư������
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

	m_pFloatWindow = new CFloatWindow;
	m_pFloatWindow->StickWndToDesktop(m_hWnd);
	
	HMODULE hModule = LoadLibrary(_T("dwmapi.dll"));
	if (hModule != NULL)
	{
		typedef HRESULT (WINAPI* DwmSetWindowAttributeFn)(HWND, DWORD, LPCVOID, DWORD);
		DwmSetWindowAttributeFn pFnDwmSetWindowAttribute = (DwmSetWindowAttributeFn)GetProcAddress(hModule, "DwmSetWindowAttribute");
		if (pFnDwmSetWindowAttribute != NULL)
		{
			DWMNCRENDERINGPOLICY RenderPolicy = DWMNCRP_ENABLED;

			//����ö��ֵDWMWA_EXCLUDED_FROM_PEEKֻ������v7.0��SDK�У�v6aSDK�����ڣ���ֱ��ʹ����ֵ
			//DwmSetWindowAttribute(m_hWnd, 12, &renderPolicy, sizeof(int));
			pFnDwmSetWindowAttribute(m_hWnd, 12, &RenderPolicy, sizeof(int));
		}
		FreeLibrary(hModule);
	}

	/*CLabelUI* pText = (CLabelUI*)m_PaintManager.FindControl(_T("News"));
	UINT nUnicode[] = {0x06DE};
	pText->SetText((LPCTSTR)nUnicode);
*/
	//���ô�����ʾλ��
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

	//��ʼ��Head�ؼ�ָ��
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
		//�������λ����Ϣ
		m_strCityLocation = CityInfoParam.strJsonString;
		//���λ����Ϣ��������������»�ȡ�����ͳ��п�������
		if (m_pWeatherInfo)
		{
			m_pWeatherInfo->GetCityWeather(CityInfoParam.strJsonString);
			m_pWeatherInfo->GetCityAQIInfo(CityInfoParam.strJsonString);
		}
	}
	else if (CityInfoParam.CityInfoChanged == CITYINFO_CHANGED_TEMPERATURE)
	{
		//����������Ϣ��ȡ�ɹ�
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
		//����Duilib�����࣬�ڴ��ڹر�ʱ�����Զ�����OnFinalMessage()����ʱ���Զ�delete����
		//������ùرմ��ڲŻᴥ�������ֱ��PostQuitMessage�򲻻ᴥ��
		m_pTxMiniSkin = new CTxMiniSkin(m_hWnd);

		UINT nRet = m_pTxMiniSkin->ShowModal();

		if (nRet == IDOK)
		{
			Close(IDOK);
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
			//�Ժ�ÿ10����ˢ��һ��
			m_PaintManager.SetTimer(pWeatherInfo, WEATHER_UPDATE_TIMERD_ID, 10*60*1000);
		}
	}
	else if (msg.wParam == WEATHER_UPDATE_TIMERD_ID)
	{
		if (m_pWeatherInfo != NULL)
		{
			//���������ϢΪ�գ���ô���»�ȡ������Ϣ
			if (m_strCityLocation.IsEmpty() != FALSE)
				m_pWeatherInfo->GetCityLocation();
			else
			{
				m_pWeatherInfo->GetCityWeather(m_strCityLocation);
				m_pWeatherInfo->GetCityAQIInfo(m_strCityLocation);
			}
		}
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
		DWORD dwTenKey = SysTime.wHour/10;				//�ж�Сʱ��ʮλ���Ƿ���2
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

	//��ʾ����
	if (m_SysTime.wMonth != SysTime.wMonth || m_SysTime.wDay != SysTime.wDay)
	{
		CLabelUI* pDate = (CLabelUI*)m_PaintManager.FindControl(_T("Date"));
		if (pDate)
		{
			CDuiString strDate;
			strDate.Format(_T("%d��%d��"), SysTime.wMonth, SysTime.wDay);
			pDate->SetText(strDate);
		}

		//��ʾ����
		CLabelUI* pLunarDay = (CLabelUI*)m_PaintManager.FindControl(_T("LunarDay"));
		if (pLunarDay)
		{
			WORD wYear,wMonth,wDay;
			BOOL bLeapMonth = 0;
			CLunar Lunar;
			Lunar.GetLunarDayInfo(SysTime.wYear, SysTime.wMonth, SysTime.wDay, wYear, wMonth, wDay, bLeapMonth);

			TCHAR szMonth[] = _T("�������������߰˾�ʮ����");

			TCHAR szLunarDay[10] = {0};
			CLunar::FormatLunarDay(wDay, szLunarDay);

			CDuiString strLunarTime;
			strLunarTime.Format(_T("%c��%s"), szMonth[wMonth-1], szLunarDay);

			pLunarDay->SetText(strLunarTime);
		}

		m_SysTime.wMonth = SysTime.wMonth;
		m_SysTime.wDay = SysTime.wDay;
	}

	//��ʾ����
	if (m_SysTime.wDayOfWeek != SysTime.wDayOfWeek)
	{
		CLabelUI* pDayOfWeek = (CLabelUI*)m_PaintManager.FindControl(_T("DayOfWeek"));
		if (pDayOfWeek)
			//��������
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
    
	//���û�л�ȡ������������Ϣ������������������ϸ��Ϣ
    if (pHover->GetName() == _T("Temperature"))
    {
		CDuiString strCityTempture = pHover->GetText();
		if (strCityTempture.CompareNoCase(_T("δ֪")) == 0)
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

				if (ptClientPos.x == 0 && ptClientPos.y == 0)
					break;

				TCHAR szModuleFile[MAX_PATH];
				GetModuleFileName(NULL, szModuleFile, _countof(szModuleFile));
				PathRenameExtension(szModuleFile, _T(".ini"));

				//����������Ļ�������ҵ�λ��д�������ļ���
				//��߾�����Ļ�ľ���
				TCHAR szClientPos[10];
				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), ptClientPos.x);
				WritePrivateProfileString(_T("POSITION"), _T("left"), szClientPos, szModuleFile);

				//�ϱ߾�����Ļ�ľ���
				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), ptClientPos.y);
				WritePrivateProfileString(_T("POSITION"), _T("top"), szClientPos, szModuleFile);

				RECT rcWnd;
				GetWindowRect(m_hWnd,&rcWnd);

				//�ұ߾�����Ļ�ľ���
				_stprintf_s(szClientPos, _countof(szClientPos), _T("%d"), GetSystemMetrics(SM_CXSCREEN) - (ptClientPos.x + rcWnd.right - rcWnd.left));
				WritePrivateProfileString(_T("POSITION"), _T("right"), szClientPos, szModuleFile);
				//�±߾�����Ļ�ľ���
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

				if (nRight < nLeft)	//��߾�����Ļ�ľ�������ұ߾�����Ļ�ľ���
					nLeft = dwWidth - ( rcWnd.right - rcWnd.left ) - nRight;

				if (nTop > nBottom)	//�±߾�����Ļ�ľ�������ϱ߾�����Ļ�ľ���
					nTop = dwHegiht - (rcWnd.bottom - rcWnd.top) - nBottom;

				SetWindowPos(m_hWnd, NULL, nLeft, nTop, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
			}
			break;
	}

	return FALSE;
}