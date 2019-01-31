#include "StdAfx.h"
#include "SystemTray.h"

#include "DllCore/Utils/RemoteProcess.h"

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

BOOL GetTrayIcon(HWND hWndTray, CTrayDataInfo& TrayDataInfoList)
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

BOOL GetTrayIcon64(HWND hWndTray, CTrayDataInfo& TrayDataInfoList)
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