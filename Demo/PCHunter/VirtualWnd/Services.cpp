#include "stdafx.h"

CServices::CServices()
{
	//
}

CServices::~CServices()
{

}

void CServices::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_MENU)
		OnMenu(msg);
	else
		__super::Notify(msg);
}

BOOL CServices::GetSvrInfo(CSrvInfoList& SvrInfoList)
{
	BOOL bSuccess = FALSE;
	SC_HANDLE hSCMag = NULL;
	LPENUM_SERVICE_STATUS_PROCESS lpEss = NULL;
	do 
	{
		SERVICEINFO SvrInfo;
		LPQUERY_SERVICE_CONFIG lpSC;
		DWORD dwBytesNeeded=0, cbBufSize=0, dwReslut=0, dwResume=0; 

		LPSERVICE_DESCRIPTION lpsd;
		DWORD cbBufSize2=0, dwNeeded=0;

		hSCMag = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
		if (hSCMag == NULL)
			break;

		if ( EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, 
			&dwBytesNeeded, &dwReslut, &dwResume, NULL)  == FALSE)
		{		
			lpEss = (LPENUM_SERVICE_STATUS_PROCESS) new BYTE[dwBytesNeeded];
			if (lpEss == NULL)
				break;
		}
		// 枚举服务，得到服务的进程ID，服务名
		EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (LPBYTE)lpEss, dwBytesNeeded, 
			&dwBytesNeeded, &dwReslut, &dwResume, NULL);

		for (UINT i=0; i<dwReslut; ++i)
		{
			SC_HANDLE hSCSvr = OpenService( hSCMag, lpEss[i].lpServiceName, SERVICE_QUERY_CONFIG); 	
			if (hSCSvr == NULL)
				break;

			TCHAR szBuffer[1024]={0};
			QueryServiceConfig( hSCSvr,(LPQUERY_SERVICE_CONFIG)szBuffer, 1024, &dwBytesNeeded);
			lpSC = (LPQUERY_SERVICE_CONFIG)szBuffer;

			TCHAR szBuffer2[1024]={0};
			QueryServiceConfig2( hSCSvr, SERVICE_CONFIG_DESCRIPTION, (LPBYTE) szBuffer2, 1024, &dwBytesNeeded);
			lpsd = (LPSERVICE_DESCRIPTION)szBuffer2;

			SvrInfo.strSvrName = lpEss[i].lpServiceName;
			SvrInfo.strDisplayName = lpEss[i].lpDisplayName;
		    SvrInfo.strPath = CString (lpSC->lpBinaryPathName);
			SvrInfo.strDescription = lpsd->lpDescription;

			/*while(lpSC->lpDependencies[0] != NULL)
			{
				SvrInfo.DependList.AddTail(lpSC->lpDependencies);
				lpSC->lpDependencies += _tcslen(lpSC->lpDependencies) + 1;
			}*/
			
			SvrInfo.dwPID = lpEss[i].ServiceStatusProcess.dwProcessId;
			SvrInfo.dwRunStatus = (SVR_STATE)lpEss[i].ServiceStatusProcess.dwCurrentState;
			SvrInfo.dwStartType = (SVR_STARTTYPE)lpSC->dwStartType;
			SvrInfo.dwType = (SVR_TYPE)lpEss[i].ServiceStatusProcess.dwServiceType;
			
			SvrInfoList.AddTail(SvrInfo);

			CloseServiceHandle(hSCSvr);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hSCMag != NULL)
		CloseServiceHandle(hSCMag);
	if (lpEss != NULL)
	{
		delete[] lpEss;
		lpEss = NULL;
	}

	return bSuccess;
}

void CServices::OnPaint()
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Service"));
	if (pList->GetCount())
		pList->RemoveAll();
	CSrvInfoList SvrInfoList;
	if (GetSvrInfo(SvrInfoList) == FALSE)
		return;

	if (SvrInfoList.GetCount() == 0)
		return;

	POSITION pos = SvrInfoList.GetHeadPosition();
	int i=0;
	CDuiString strFormatText;
	while(pos)
	{
		const SERVICEINFO& SvrInfo	= SvrInfoList.GetNext(pos);
		CListTextElementUI* pTextElement = new CListTextElementUI;
		pList->Add(pTextElement);

		int m=0;
		pTextElement->SetText(m++, SvrInfo.strSvrName);
		pTextElement->SetText(m++, SvrInfo.strDisplayName);

		strFormatText.Empty();

		FormatPid(SvrInfo.dwPID, strFormatText);
		pTextElement->SetText(m++, strFormatText);

		FormatRunStatus(SvrInfo.dwRunStatus, strFormatText);
		pTextElement->SetText(m++, strFormatText);

		FormatStartType(SvrInfo.dwStartType, strFormatText);
		pTextElement->SetText(m++, strFormatText);

		pTextElement->SetText(m++, SvrInfo.strPath);
		pTextElement->SetText(m++, SvrInfo.strDescription);
		
		//if (SvrInfo.DependList.GetCount() != 0)
		//	pTextElement->SetUserData(_T("true"));
	}
}

void CServices::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("Service"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		if (pList->GetCurSel() == -1)
			return;

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(_T("SvrMenu.xml"));
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);

		//获取选中行
		int nItem = pList->GetCurSel();
		CListTextElementUI* pItem = (CListTextElementUI*)pList->GetItemAt(nItem);
		if (pItem == NULL)
			return;

		CMenuUI* pMenuRoot = (CMenuUI*)pMenu->GetMenuUI();
		CMenuElementUI* pMenuElement = (CMenuElementUI*)pMenu->m_pm.FindControl(_T("RelayOnSvr"));

		if (pMenuElement != NULL && _tcsicmp(pItem->GetUserData(), _T("true")) != 0)
		{
			pMenuRoot->Remove(pMenuElement);
			pMenu->ResizeMenu();
		}
	}
}

void CServices::FormatPid(DWORD Pid, CDuiString& strPid)
{
	if (Pid != 0)
		strPid.Format(_T("%u"), Pid);
}

void CServices::FormatRunStatus(DWORD dwCurrentState, CDuiString& strRunState)
{
	TCHAR szRunState[MAX_PATH];
	switch (dwCurrentState)
	{
		case START_PENDING:
				_stprintf_s(szRunState, _countof(szRunState), _T("正在启动"));
			break;
		case STOP_PENDING:
				_stprintf_s(szRunState, _countof(szRunState), _T("正在停止"));
			break;
		case RUNNING:
				_stprintf_s(szRunState, _countof(szRunState), _T("正在运行"));
			break;
		case CONTINUE_PENDING:
				_stprintf_s(szRunState, _countof(szRunState), _T("正在继续"));
			break;
		case PAUSE_PENDING:
				_stprintf_s(szRunState, _countof(szRunState), _T("正在暂停"));
			break;
		default:
			szRunState[0] = _T('\0');
	}

	strRunState = szRunState;
}

void CServices::FormatStartType(DWORD dwStartType, CDuiString& strStartType)
{
	TCHAR szStartType[MAX_PATH];
	switch (dwStartType)
	{
		case BOOT_START:
				_stprintf_s(szStartType, _countof(szStartType), _T("引导启动"));
			break;
		case SYSTEM_START:
				_stprintf_s(szStartType, _countof(szStartType), _T("系统启动"));
			break;
		case AUTO_START:
				_stprintf_s(szStartType, _countof(szStartType), _T("自动启动"));
			break;
		case DEMAND_START:
				_stprintf_s(szStartType, _countof(szStartType), _T("手动启动"));
			break;
		case DISABLED_START:
				_stprintf_s(szStartType, _countof(szStartType), _T("禁止启动"));
			break;
		default:
			szStartType[0] = _T('\0');
	}

	strStartType = szStartType;
}