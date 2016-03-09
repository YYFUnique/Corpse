#include "stdafx.h"
#include "Resource.h"
#include "Calendar.h"

#define	CALENDAR_START_YEAR	2000
#define	CALENDAR_END_YEAR		2038

CCalendar::CCalendar()
{

}

CCalendar::~CCalendar()
{

}

LPCTSTR CCalendar::GetWindowClassName() const
{
	return _T("CalendarMgr");
}

UILIB_RESOURCETYPE CCalendar::GetResourceType() const
{
	return UILIB_FILE;
}

LPCTSTR CCalendar::GetResourceID() const
{
	return _T("");
}

CDuiString CCalendar::GetZIPFileName() const
{
	return _T("Calendar.zip");
}

CDuiString CCalendar::GetSkinFile()
{
	return _T("Calendar.xml");
}

CDuiString CCalendar::GetSkinFolder()
{
	return _T("Calendar");
}

void CCalendar::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
		OnItemClick(msg);
}

void CCalendar::InitWindow()
{
	SetIcon(IDI_MAINFRAME);	
	
	CTime Time = CTime::GetCurrentTime();
	m_nYear = Time.GetYear();
	m_nMonth = Time.GetMonth();

	OnInitControl();

	BuildCalendar(m_nYear,m_nMonth);
}

void CCalendar::BuildCalendar(UINT nYear,UINT nMonth)
{
	CCalendarUI* pCalendar = (CCalendarUI*)m_PaintManager.FindControl(_T("Calendar"));
	ASSERT(pCalendar);

	pCalendar->RemoveAll();
	CALENDAR_TIME CalendarTime;
	CalendarTime.nYear = nYear;
	CalendarTime.nMonth = nMonth;
	CalendarTime.bInMonth = false;

	pCalendar->SetCurrentYear(CalendarTime.nYear);
	pCalendar->SetCurrentMonth(CalendarTime.nMonth);
	UINT nLastMonth = pCalendar->GetCurrentMonth() -1;
	if (nLastMonth == 0)
		nLastMonth = 12;

	UINT nLastMonthDay = pCalendar->GetDayCount(12);
	UINT nDayWeek = pCalendar->GetFirstDayWeek();
	UINT nCountDay = pCalendar->GetDayCount();
	//上一月天数，在本月以灰色显示上一月
	//以星期一为第一天、周日为最后一天为例，其他情况可能有所改变
	CalendarTime.bInMonth = false;
	if (nDayWeek == 1 || nDayWeek == 2)		//计算在本月显示的上一月的号数
		nDayWeek += 7;
	for (UINT nDay=nLastMonthDay - nDayWeek +3 ;nDay<=nLastMonthDay;++nDay)
	{
		CalendarTime.nDay = nDay;
		pCalendar->AddNode((LPARAM)&CalendarTime,_T("EveryDay.xml"));
	}
	CalendarTime.bInMonth = true;
	for (UINT nDay=1;nDay<nCountDay+1;++nDay)
	{
		CalendarTime.nDay = nDay;
		UINT nMod = (nDay -1 + nDayWeek)%7;		//根据当月1号计算这个月是否是周末
		if (nMod == 1 || nMod ==	0)
			CalendarTime.bWeekend = true;
		else
			CalendarTime.bWeekend = false;
		pCalendar->AddNode((LPARAM)&CalendarTime,_T("EveryDay.xml"));
	}

	CalendarTime.bInMonth = false;
	//确保可以显示6周时间
	UINT nDay= nDayWeek - 3 + nCountDay;
	UINT nWeek = 7;
	if (nDay<=35)
		nWeek = 7*2;
	
	UINT nDays = nWeek-nDay%7;
	for (UINT nDay=1;nDay<nDays;++nDay)
	{
		CalendarTime.nDay = nDay;
		pCalendar->AddNode((LPARAM)&CalendarTime,_T("EveryDay.xml"));
	}
}

LRESULT CCalendar::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

//LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
CControlUI* CCalendar::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pControl = NULL;
	if (_tcsicmp(pstrClass,_T("CalendarUI")) == 0)
	{
		SIZE ItemSize = {58,60};
		pControl = new CCalendarUI(m_PaintManager,ItemSize);
	}
	return pControl;
}

void CCalendar::OnInitControl()
{
	CComboUI* pYear = (CComboUI*)m_PaintManager.FindControl(_T("Year"));
	for (UINT nStart = CALENDAR_START_YEAR;nStart<CALENDAR_END_YEAR;++nStart)
	{
		CListLabelElementUI* pListLabel = new CListLabelElementUI;
		pYear->Add(pListLabel);
		pListLabel->SetFixedHeight(25);
		TCHAR szYear[10];
		_stprintf_s(szYear,_countof(szYear),_T("%d"),nStart);
		pListLabel->SetText(szYear);
		if (nStart == m_nYear)
			pListLabel->Select(true);
	}

	CComboUI* pMonth = (CComboUI*)m_PaintManager.FindControl(_T("Month"));
	for(UINT nStart=1;nStart<=12;++nStart)
	{
		CListLabelElementUI* pListLabel = new CListLabelElementUI;
		pMonth->Add(pListLabel);
		pListLabel->SetFixedHeight(25);
		TCHAR szMonth[10];
		_stprintf_s(szMonth,_countof(szMonth),_T("%02d"),nStart);
		pListLabel->SetText(szMonth);
		if (nStart == m_nMonth)
			pListLabel->Select(true);
	}
}

void CCalendar::OnClick(TNotifyUI& msg)
{
	CDuiString strSenderName = msg.pSender->GetName();
	if (strSenderName == _T("BtnClose"))
		PostQuitMessage(0);
	else if (strSenderName == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (strSenderName == _T("BtnToday"))
	{
		CTime Time = CTime::GetCurrentTime();
		m_nYear = Time.GetYear();
		m_nMonth = Time.GetMonth();

		CComboUI* pYear = (CComboUI*)m_PaintManager.FindControl(_T("Year"));
		pYear->SelectItem(m_nYear-CALENDAR_START_YEAR);
		CComboUI* pMonth = (CComboUI*)m_PaintManager.FindControl(_T("Month"));
		pMonth->SelectItem(m_nMonth-1);
	}
	else if (strSenderName == _T("BtnPrev")){
		m_nMonth -= 1;
		if (m_nMonth == 0)
		{
			m_nMonth = 12;
			m_nYear -= 1;
			CComboUI* pYear = (CComboUI*)m_PaintManager.FindControl(_T("Year"));
			pYear->SelectItem(m_nYear - CALENDAR_START_YEAR);
		}
		CComboUI* pMonth = (CComboUI*)m_PaintManager.FindControl(_T("Month"));
		pMonth->SelectItem(m_nMonth -1);
	}
	else if (strSenderName == _T("BtnNext")){
		m_nMonth += 1;
		if (m_nMonth == 13)
		{
			m_nMonth = 1;
			m_nYear += 1;
			CComboUI* pYear = (CComboUI*)m_PaintManager.FindControl(_T("Year"));
			pYear->SelectItem(m_nYear - CALENDAR_START_YEAR);
		}
		CComboUI* pMonth = (CComboUI*)m_PaintManager.FindControl(_T("Month"));
		pMonth->SelectItem(m_nMonth -1);
	}
}

void CCalendar::OnItemClick(TNotifyUI& msg)
{
	CDuiString& strNotifyName = msg.pSender->GetName();
	if (strNotifyName == _T("Year"))
	{
		m_nYear = _ttoi(msg.pSender->GetText());
		BuildCalendar(m_nYear,m_nMonth);
	}
	else if (strNotifyName == _T("Month"))
	{
		m_nMonth = _ttoi(msg.pSender->GetText());
		BuildCalendar(m_nYear,m_nMonth);
	}
	else if (strNotifyName == _T("HolidayPlan"))
	{

	}
}