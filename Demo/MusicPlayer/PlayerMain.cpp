#include "stdafx.h"
#include "PlayerMain.h"
#include "Resource.h"

#define		WM_TRAYICON					(WM_USER+0x1000)		//托盘图标鼠标消息

CPlayerMain::CPlayerMain()
	: m_pTrayIcon(NULL)
{

}

CPlayerMain::~CPlayerMain()
{
	if (m_pTrayIcon)
	{
		
		delete m_pTrayIcon;
		m_pTrayIcon = NULL;
	}
}

// DUI_BEGIN_MESSAGE_MAP(CPlayerMain,WindowImplBase)
// 
// DUI_END_MESSAGE_MAP()


LPCTSTR CPlayerMain::GetWindowClassName() const
{
	return _T("DUIMainFrame");  
}

CDuiString CPlayerMain::GetSkinFile()
{
	return _T("PlayerMain.xml");
}

CDuiString CPlayerMain::GetSkinFolder()
{
	return _T("Player");
}

void CPlayerMain::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

CControlUI* CPlayerMain::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT CPlayerMain::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT bRes = FALSE;
	bHandled = TRUE;
	switch(uMsg)
	{
	case WM_TRAYICON:
		bRes = OnTrayIcon(wParam,lParam);
		break;
	default:
		bHandled = FALSE;
	}

	if (bHandled) return bRes;

	return WindowImplBase::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
}

void CPlayerMain::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
	m_pTrayIcon = new CUITrayIcon;
	if (m_pTrayIcon)
	{
		m_pTrayIcon->CreateTrayIcon(m_hWnd, IDI_MAINFRAME, _T("音乐播放器"), WM_TRAYICON);
	}
	//SetClassLong(this->m_hWnd, GCL_STYLE, GetClassLong(this->m_hWnd, GCL_STYLE) | CS_DROPSHADOW);
	m_pTreeView=(CTreeViewUI*)(m_PaintManager.FindControl(_T("TreeView")));
	if(m_pTreeView!=NULL)
	{
		TListInfoUI * pListInfo=m_pTreeView->GetListInfo();
		pListInfo->sHotImage=_T("file='treeItemBack.png' corner='5,5,5,5'");
		pListInfo->sSelectedImage = _T("file='treeItemBack.png' corner='5,5,5,5'");
		pListInfo->dwBkColor=0;
		pListInfo->dwHotBkColor=0;
		pListInfo->dwSelectedBkColor=0;
	}

	m_pListView=(CListUI*)(m_PaintManager.FindControl(_T("playlist")));
	if(m_pListView!=NULL)
	{
		TListInfoUI * pListInfo=m_pListView->GetListInfo();
		pListInfo->sHotImage=_T("file='treeItemBackhot.png' corner='5,5,5,5'");
		pListInfo->sSelectedImage = _T("file='treeItemBack.png' corner='5,5,5,5'");
		pListInfo->nFont=0;
		pListInfo->dwTextColor=0xff797b7e;
		pListInfo->dwHotTextColor=0xfffefefe;
		pListInfo->dwSelectedTextColor=0xfffefe7e;
		pListInfo->dwLineColor=0xff303337;
		pListInfo->dwBkColor=0xFF303030;
		pListInfo->dwHotBkColor=0;
		pListInfo->dwSelectedBkColor=0;
		pListInfo->bShowColumnLine=false;
		pListInfo->bAlternateBk = true;
		//pListInfo->sBkImage = _T("file='listico.png' dest='5,6,20,20'");
		RECT rctTextPadding={26,2,0,2};
		pListInfo->rcTextPadding=rctTextPadding;
		for(int i=0;i<10;++i)
		{
			AddItemToList(_T("UI制造者群歌大合唱"),_T("mid"),_T("6.10MB"),_T("2014-05-01 14:01"));
			AddItemToList(_T("当我想起你的时候－汪峰"),_T("mp3"),_T("4.33MB"),_T("2014-05-01 15:21"));
			AddItemToList(_T("出卖－周传雄"),_T("mp3"),_T("5.20MB"),_T("2014-05-01 16:13"));
			AddItemToList(_T("何必在一起"),_T("mp3"),_T("3.10MB"),_T("2014-05-02 14:01"));
			AddItemToList(_T("小花－百慕三石"),_T("mid"),_T("2.10MB"),_T("2013-05-01 14:01"));
			AddItemToList(_T("杨家岭的早上"),_T("mid"),_T("512.10MB"),_T("2010-02-01 11:33"));
		}
	}
	/*m_pCombo=(CComboUI*)(m_PaintManager.FindControl(_T("select")));
	if(m_pCombo!=NULL)
	{
		TListInfoUI * pListInfo=m_pCombo->GetListInfo();
		pListInfo->sHotImage=_T("file='treeItemBackhot.png' corner='5,5,5,5'");
		pListInfo->sSelectedImage = _T("file='treeItemBack.png' corner='5,5,5,5'");
		pListInfo->nFont=0;
		pListInfo->dwTextColor=0xff797b7e;
		pListInfo->dwHotTextColor=0xfffefefe;
		pListInfo->dwSelectedTextColor=0xff797b7e;
		pListInfo->dwBkColor=0;
		pListInfo->dwHotBkColor=0;
		pListInfo->dwSelectedBkColor=0;
		RECT rctTextPadding={10,2,5,2};
		pListInfo->rcTextPadding=rctTextPadding;
	}*/
}

void CPlayerMain::AddItemToList(LPCTSTR lpszTitle,LPCTSTR lpszType,LPCTSTR lpszSize,LPCTSTR lpszModifyTime)
{
	CListTextElementUI* pListElement = new CListTextElementUI;
	m_pListView->Add(pListElement);
	pListElement->SetFixedHeight(26);
	pListElement->SetBkColor(0xff0000);
	pListElement->SetForeImage(_T("file='listico.png' dest='65,6,80,20'"));

	DWORD dwCount = m_pListView->GetCount();
	TCHAR szCount[20];
	_stprintf_s(szCount, _countof(szCount), _T("%u"), dwCount);

	pListElement->SetText(0, szCount);
	pListElement->SetText(1, lpszTitle);
	pListElement->SetText(2, lpszType);
	pListElement->SetText(3, lpszSize);
	pListElement->SetText(4, lpszModifyTime);
}

LRESULT CPlayerMain::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}

void CPlayerMain::OnClick(TNotifyUI& msg)
{
	CDuiString strCtrlName = msg.pSender->GetName();
	if (strCtrlName == _T("closebtn"))
	{
		if (m_pTrayIcon)
			m_pTrayIcon->DeleteTrayIcon();
		Close();
	}
	else if(strCtrlName == _T("minbtn"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
// 	else if(strCtrlName == _T("help"))
// 		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
}

LRESULT CPlayerMain::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	UINT uMsgId=LOWORD(lParam);

	if (uMsgId==WM_RBUTTONUP)
	{
		CMenuWnd* pMenu = new CMenuWnd();
		CDuiPoint pt;
		GetCursorPos(&pt);
		STRINGorID strXmlFile(_T("MenuStatus.xml"));
		pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager,NULL,eMenuAlignment_Left|eMenuAlignment_Bottom);
	}
	else if (uMsgId == WM_MOUSEMOVE)
		m_pTrayIcon->OnMouseMove(); 
	/*else if (uMsgId == WM_MOUSEHOVER)
	{
		if (m_pMsgTip == NULL)
			m_pMsgTip = new CMessageTip(m_hWnd,_T(""),0xFFFFFFFF,_T("我欲飞翔"),_T(""));

		m_pMsgTip->ShowWindow(true);
	}*/
	/*else if (uMsgId == WM_MOUSELEAVE)
	{
		POINT pt;
		GetCursorPos(&pt);
		RECT rcPos;
		GetWindowRect(m_pMsgTip->GetHWND(),&rcPos);
		if (PtInRect(&rcPos,pt) == TRUE)
		{
			m_Tray.SetTrackMouse(TRUE);
		}
		else if (m_pMsgTip->GetHWND() != NULL && IsWindow(m_pMsgTip->GetHWND()))
		{
			m_pMsgTip->ShowWindow(false);
		}
	}*/

	return TRUE;
}