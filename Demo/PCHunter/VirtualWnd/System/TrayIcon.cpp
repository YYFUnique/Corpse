#include "StdAfx.h"
#include "TrayIcon.h"

#include "DllCore/Utils/OsInfo.h"
#include "Utils/RemoteProcess.h"

HWND GetTrayWndHwnd()
{
	HWND hWnd = FindWindow(_T("Shell_TrayWnd"),NULL);	//获取任务栏句柄
	if (hWnd != NULL)
	{
		hWnd = FindWindowEx(hWnd,NULL,_T("TrayNotifyWnd"),NULL);	//获取右下角区域
		if (hWnd != NULL)
		{
			HWND hPageWnd = FindWindowEx(hWnd,NULL,_T("SysPager"),NULL);	//
			if (hPageWnd != NULL)
				hWnd = FindWindowEx(hPageWnd,NULL,_T("ToolBarWindow32"),NULL);
			else
				hWnd = FindWindowEx(hWnd,NULL,_T("ToolBarWindow32"),NULL);

			return hWnd;
		}
	}
	return NULL;
}

HWND GetOverflowHwnd()
{
	HWND hWnd =FindWindow(_T("NotifyIconOverflowWindow"),NULL);  //得到溢出通知区域句柄
	if (hWnd!=NULL)
		hWnd=FindWindowEx(hWnd,NULL,_T("ToolBarWindow32"),NULL);
	return hWnd;
}

CTrayIcon::CTrayIcon()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

CTrayIcon::~CTrayIcon()
{

}

DUI_BEGIN_MESSAGE_MAP(CTrayIcon, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
DUI_END_MESSAGE_MAP()

void CTrayIcon::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CTrayIcon::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	CTrayDataInfo TrayDataInfoList;
	GetTrayAreaIcon(TrayDataInfoList);
	if (OsIsVistaOrLater())
		GetOverflowAreaIcon(TrayDataInfoList);

	CDuiString strTipInfo;
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("TrayApp"));
	if (pList->GetCount() != 0)
		pList->RemoveAll();

	TListInfoUI* pListInfo = pList->GetListInfo();
	POSITION pos = TrayDataInfoList.GetHeadPosition();
	while(pos)
	{
		CFileListItemUI* pTrayItemInfo = NULL;
		const TRAYDATAINFO& TrayDataInfo = TrayDataInfoList.GetNext(pos);

		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pTrayItemInfo = (CFileListItemUI*)m_DialogBuilder.Create(TRAY_LIST_ITEM_INFO, NULL, &m_RootBuilder, m_pPaintManager);
		else
			pTrayItemInfo = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

		pList->Add(pTrayItemInfo);
		pTrayItemInfo->SetFixedHeight(27);
		pTrayItemInfo->SetTag((UINT_PTR)TrayDataInfo.hWnd);

		CPictureUI* pProcessIcon = (CPictureUI*)m_pPaintManager->FindControl(_T("Pic"));
		pProcessIcon->SetVisible(true);
		pProcessIcon->SetIcon(TrayDataInfo.hTrayIcon);

		CLabelUI* pTitle = (CLabelUI*)m_pPaintManager->FindControl(_T("Title"));
		pTitle->SetText(TrayDataInfo.strTitle);
		pTitle->SetFont(pListInfo->nFont);
		pTitle->SetForeColor(pListInfo->dwTextColor);

		DWORD dwProcessId;
		GetWindowThreadProcessId(TrayDataInfo.hWnd, &dwProcessId);
		strTipInfo.Format(_T("%d"), dwProcessId);
		CLabelUI* pPid = (CLabelUI*)m_pPaintManager->FindControl(_T("PID"));
		pPid->SetText(strTipInfo);
		pPid->SetFont(pListInfo->nFont);
		pPid->SetForeColor(pListInfo->dwTextColor);

		CLabelUI* pProcessName = (CLabelUI*)m_pPaintManager->FindControl(_T("ProcessName"));
		pProcessName->SetText(PathFindFileName(TrayDataInfo.strFilePath));
		pProcessName->SetFont(pListInfo->nFont);
		pProcessName->SetForeColor(pListInfo->dwTextColor);

		strTipInfo.Format(_T("0X%08X"), TrayDataInfo.hWnd);
		CLabelUI* pHwnd = (CLabelUI*)m_pPaintManager->FindControl(_T("Hwnd"));
		pHwnd->SetText(strTipInfo);
		pHwnd->SetFont(pListInfo->nFont);
		pHwnd->SetForeColor(pListInfo->dwTextColor);

		TCHAR szWinTitle[MAX_PATH];
		::GetWindowText(TrayDataInfo.hWnd, szWinTitle,_countof(szWinTitle));
		CLabelUI* pWinTitle = (CLabelUI*)m_pPaintManager->FindControl(_T("WinTitle"));
		pWinTitle->SetText(szWinTitle);
		pWinTitle->SetFont(pListInfo->nFont);
		pWinTitle->SetForeColor(pListInfo->dwTextColor);

		strTipInfo.Format(_T("%d"), TrayDataInfo.uID);
		CLabelUI* pTrayId = (CLabelUI*)m_pPaintManager->FindControl(_T("uID"));
		pTrayId->SetText(strTipInfo);
		pTrayId->SetFont(pListInfo->nFont);
		pTrayId->SetForeColor(pListInfo->dwTextColor);

		strTipInfo.Format(_T("WM_USER+%d"), TrayDataInfo.uCallbackMessage - WM_USER);
		CLabelUI* pTrayMsgId = (CLabelUI*)m_pPaintManager->FindControl(_T("MsgID"));
		pTrayMsgId->SetText(strTipInfo);
		pTrayMsgId->SetFont(pListInfo->nFont);
		pTrayMsgId->SetForeColor(pListInfo->dwTextColor);
	}
}

void CTrayIcon::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("TrayApp"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(TRAY_MENU);
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
	}
}

void CTrayIcon::OnRefresh(TNotifyUI& msg)
{
	m_bLoad = FALSE;
	OnLoadItem(msg);
}

void CTrayIcon::OnTrayMenu(CControlUI* pControl)
{
	CListUI* pAppList = (CListUI*)m_pPaintManager->FindControl(_T("TrayApp"));
	if (pAppList->GetCurSel() == INVALID_ITEM_INDEX)
		return;

	CListTextElementUI* pItem = (CListTextElementUI*)pAppList->GetItemAt(pAppList->GetCurSel());

	HWND hWnd = (HWND)pItem->GetTag();
	if (IsWindow(hWnd) == FALSE)
		return;

	CDuiString strItemName = pControl->GetName();
	if (strItemName.CompareNoCase(_T("Show")) != 0 ) {

	} else if (strItemName.CompareNoCase(_T("LButton")) != 0) {

	} else if (strItemName.CompareNoCase(_T("LDButton")) != 0) {

	} else if (strItemName.CompareNoCase(_T("RButton")) != 0) {

	} else if (strItemName.CompareNoCase(_T("Up")) != 0) {

	} else if (strItemName.CompareNoCase(_T("Down")) != 0) {

	}

}

BOOL CTrayIcon::GetTrayAreaIcon(CTrayDataInfo& TrayDataInfo)
{
	HWND hWndTray = GetTrayWndHwnd();
	if (::IsWindow(hWndTray) == FALSE)
		return FALSE;

	////////////////////////////////////////////////////////////////////////
	////// 由于32位进程读取64位系统进程数据，64位系统进程返回的
	////// 是64位进程格式的结构体数据，故需要分类处理。
	////////////////////////////////////////////////////////////////////////
	if (OsIsWow64Process())
		return GetTrayIcon64(hWndTray, TrayDataInfo);
	return GetTrayIcon(hWndTray, TrayDataInfo);
}

BOOL CTrayIcon::GetOverflowAreaIcon(CTrayDataInfo& TrayDataInfo)
{
	HWND hWndTray = GetOverflowHwnd();
	if (::IsWindow(hWndTray) == FALSE)
		return FALSE;

	////////////////////////////////////////////////////////////////////////
	////// 由于32位进程读取64位系统进程数据，64位系统进程返回的
	////// 是64位进程格式的结构体数据，故需要分类处理。
	////////////////////////////////////////////////////////////////////////
	if (OsIsWow64Process())
		return GetTrayIcon64(hWndTray, TrayDataInfo);
	return GetTrayIcon(hWndTray, TrayDataInfo);
}

BOOL CTrayIcon::GetTrayIcon(HWND hWndTray, CTrayDataInfo& TrayDataInfoList)
{
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWndTray, &dwProcessId);

	TBBUTTON TbBtn;
	CRemoteProcess<TBBUTTON> ProcessInfo(dwProcessId);
	BOOL bSuccess = FALSE;

	do 
	{
		BYTE bTrayData[1024];
		//获取图标个数
		int lButton = SendMessage(hWndTray, TB_BUTTONCOUNT, 0, 0);
		for (int n=0; n<lButton; ++n)
		{
			//	获取第n个图标信息地址
			SendMessage(hWndTray, TB_GETBUTTON, n, (LPARAM)ProcessInfo.GetData());

			if (ProcessInfo.ReadData(&TbBtn) != FALSE)
			{
				if (ProcessInfo.ReadData<BYTE, _countof(bTrayData)>(bTrayData, (LPCVOID)TbBtn.dwData) != FALSE)
				{
					TRAYDATAFIREWALL* pTrayData = (TRAYDATAFIREWALL*)bTrayData;

					TRAYDATAINFO TrayDataInfo;

					TrayDataInfo.hWnd = (HWND)pTrayData->hMainWnd;
					TrayDataInfo.uCallbackMessage = pTrayData->uCallbackMessage;
					TrayDataInfo.hTrayIcon = pTrayData->hIcon;
					TrayDataInfo.uID = pTrayData->uID;
					TrayDataInfo.strFilePath = pTrayData->szExePath;
					TrayDataInfo.strTitle = pTrayData->szTip;
					TrayDataInfo.bShowTray = (TbBtn.fsState & TBSTATE_HIDDEN) == TBSTATE_HIDDEN ? TRUE : FALSE;
					TrayDataInfoList.AddTail(TrayDataInfo);
				}
			}
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CTrayIcon::GetTrayIcon64(HWND hWndTray, CTrayDataInfo& TrayDataInfoList)
{
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWndTray, &dwProcessId);
	
	TBBUTTON64 TbBtn;
	CRemoteProcess<TBBUTTON64> ProcessInfo(dwProcessId);
	BOOL bSuccess = FALSE;

	do 
	{
		DWORD_PTR lpDataAddr = 0;
		BYTE bTrayData[1024];
		//获取图标个数
		int nButton = SendMessage(hWndTray, TB_BUTTONCOUNT, 0, 0);
		for (int n=0; n<nButton; ++n)
		{
			//	获取第n个图标信息地址
			SendMessage(hWndTray, TB_GETBUTTON, n, (LPARAM)ProcessInfo.GetData());

			if (ProcessInfo.ReadData(&TbBtn) != FALSE)
			{
				if (ProcessInfo.ReadData<BYTE,_countof(bTrayData)>(bTrayData, (LPCVOID)TbBtn.dwData) != FALSE)
				{
					PTRAYDATAFIREWALL64 pTrayData = (PTRAYDATAFIREWALL64)bTrayData;
					TRAYDATAINFO TrayDataInfo;
					
					TrayDataInfo.hWnd = (HWND)pTrayData->hMainWnd;
					TrayDataInfo.uCallbackMessage = pTrayData->uCallbackMessage;
					TrayDataInfo.hTrayIcon = pTrayData->hIcon;
					TrayDataInfo.uID = pTrayData->uID;
					TrayDataInfo.strFilePath = pTrayData->szExePath;
					TrayDataInfo.strTitle = pTrayData->szTip;
					TrayDataInfo.bShowTray = (TbBtn.fsState & TBSTATE_HIDDEN) == TBSTATE_HIDDEN ? TRUE : FALSE;
					TrayDataInfoList.AddTail(TrayDataInfo);
				}
			}
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}