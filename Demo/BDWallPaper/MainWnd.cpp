#include "StdAfx.h"
#include "MainWnd.h"

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CMainPage, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

CMainPage::CMainPage()
{
	m_pPaintManager = NULL;
}

void CMainPage::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CMainPage::OnClick(TNotifyUI& msg)
{
	
}

void CMainPage::OnSelectChanged( TNotifyUI &msg )
{

}

void CMainPage::OnItemClick( TNotifyUI &msg )
{

}

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CMainWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

CMainWnd::CMainWnd(void)
{
	m_MainPage.SetPaintMagager(&m_PaintManager);

	AddVirtualWnd(_T("mainpage"),&m_MainPage);
}

CMainWnd::~CMainWnd(void)
{
	RemoveVirtualWnd(_T("mainpage"));
}

DuiLib::CDuiString CMainWnd::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("skin\\BDWallPaper\\");
#else
	return _T("skin\\");
#endif
}

DuiLib::CDuiString CMainWnd::GetSkinFile()
{
	return _T("main.xml");
}

UILIB_RESOURCETYPE CMainWnd::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIPRESOURCE;
#endif
}

LPCTSTR CMainWnd::GetResourceID() const
{
	return _T("IDR_SKIN");
}

DuiLib::CDuiString CMainWnd::GetZIPFileName() const
{
	return _T("skin.zip");
}

LPCTSTR CMainWnd::GetWindowClassName( void ) const
{
	return _T("MainWnd");
}

CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass)
{
	return ::CreateControl(pstrClass);
}

void CMainWnd::OnFinalMessage( HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

struct TABBTN
{
	CString sType;
	CString sText;
};
void CMainWnd::InitWindow()
{
	// ��ʼ�����ݹ�����
	m_Data.Init(m_hWnd);
	// �������ఴť
	CHorizontalLayoutUI* pTabBar = (CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("tabbar"));
	if(pTabBar)
	{
		TABBTN aTypes[] = {
			{_T("8009|1003|1004|1007|1002|1005|1006|1001|1008"), _T("��ѡ")},
			{_T("1003"), _T("�羰")},
			{_T("1004"), _T("��Ů")},
			{_T("1007"), _T("С����")},
			{_T("1002"), _T("����")},
			{_T("1005"), _T("�ȳ�")},
			{_T("1006"), _T("����")},
			{_T("1001"), _T("����")},
			{_T("1008"), _T("����")},
			{_T("8009"), _T("��ɫ")},
		};
		COptionUI* pFirst = NULL;
		for(int i = 0; i < _countof(aTypes); ++i)
		{
			TABBTN Tab = aTypes[i];
			COptionUI* pOption = new COptionUI();
			pOption->SetName(_T("wp_tabbtn"));
			pOption->SetText(Tab.sText);
			pOption->SetUserData(Tab.sType);
			pOption->SetGroup(_T("tabbar"));
			pOption->SetFixedHeight(25);
			pOption->SetFixedWidth(65);
			pOption->SetHotImage(_T("file='main/224.bmp' corner='4,4,4,4'"));
			pOption->SetSelectedImage(_T("file='main/224.bmp' corner='4,4,4,4'"));
			pTabBar->Add(pOption);
			if(pFirst == NULL) pFirst = pOption;
		}

		if(pFirst != NULL) pFirst->Selected(true);
	}

	m_Data.LoadBizhi(_T("8009|1003|1004|1007|1002|1005|1006|1001|1008"), 1, 10);
}

void CMainWnd::Notify( TNotifyUI &msg )
{
	if(msg.sType.CompareNoCase(DUI_MSGTYPE_SCROLL) == 0)
	{
		CVerticalLayoutUI* pTabPanel = (CVerticalLayoutUI*)msg.pSender;
		CString sName = msg.pSender->GetName();
		int nPage = (int)msg.wParam;
		CString sTabPanel = sName;
		int nIdx = sTabPanel.Find(_T("@"));
		if(nIdx == -1) return;
		CString sType = sTabPanel.Mid(nIdx + 1);
		if(nPage < 1) nPage = 1;
		if(nPage > 200) nPage = 200;
		
		int nScrollPos = (nPage - 1) * pTabPanel->GetVerticalScrollBar()->GetLineSize();
		SIZE szSize = {0, nScrollPos};

		pTabPanel->SetScrollPos(szSize);

		m_Data.LoadBizhi(sType, nPage, 10);
	}
	else if(msg.sType.CompareNoCase(DUI_MSGTYPE_SELECTCHANGED) == 0)
	{
		COptionUI* pTabBtn = (COptionUI*)msg.pSender;
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("wp_tablayout"));
		CString sName = pTabBtn->GetName();
		if(sName == _T("wp_tabbtn") && pTabLayout)
		{
			CString sType = pTabBtn->GetUserData();
			CString sTabPanel;
			sTabPanel.Format(_T("wp_tabpanel@%s"), sType);
			CVerticalLayoutUI* pTabPanel = (CVerticalLayoutUI*)m_PaintManager.FindControl(sTabPanel);
			if(pTabPanel == NULL)
			{
				m_Data.LoadBizhi(sType, 1, 10);
			}
			else
			{
				pTabLayout->SelectItem(pTabPanel);
			}
		}
	}

	return WindowImplBase::Notify(msg);
}

void CMainWnd::OnClick( TNotifyUI &msg )
{
	CDuiString sCtrlName = msg.pSender->GetName();
	if( sCtrlName == _T("closebtn") )
	{
		Close();
		return; 
	}
	else if( sCtrlName == _T("minbtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
		return; 
	}
	else if( sCtrlName == _T("maxbtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
		return; 
	}
	else if( sCtrlName == _T("restorebtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
		return; 
	}
}

void CMainWnd::OnSelectChanged( TNotifyUI &msg )
{
	
}

void CMainWnd::OnItemClick( TNotifyUI &msg )
{
	
}