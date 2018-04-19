#include "stdafx.h"
#include "CoreDriver.h"
#include "DllCore/Utils/ServiceControl.h"

CCoreDriver::CCoreDriver()
{
	//
}

CCoreDriver::~CCoreDriver()
{

}

DUI_BEGIN_MESSAGE_MAP(CCoreDriver, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE, OnItemActive)
DUI_END_MESSAGE_MAP()

void CCoreDriver::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CCoreDriver::OnMenu(TNotifyUI& msg)
{

}

void CCoreDriver::OnRefresh(TNotifyUI& msg)
{

}

void CCoreDriver::OnLoadItem(TNotifyUI& msg)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Driver"));
	if (pList->GetCount())
		pList->RemoveAll();

	CSrvInfoList SvrInfoList;
	if (GetDriverInfo(SvrInfoList) == FALSE)
		return;

	if (SvrInfoList.GetCount() == 0)
		return;

	TListInfoUI* pListInfo = pList->GetListInfo();
	//去掉DrawText函数中&被转义为_,比如&a会显示为'下划线a'
	pList->SetItemTextStyle(pListInfo->uTextStyle & ~DT_NOPREFIX);
	POSITION pos = SvrInfoList.GetHeadPosition();

	while(pos)
	{
		const SERVICEINFO& SvrInfo	= SvrInfoList.GetNext(pos);

		CFileListItemUI* pFileItem = NULL;
		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(DRIVER_LIST_ITEM_INFO, 0, &m_RootBuilder, m_pPaintManager);
		else
			pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

		pList->Add(pFileItem);
		pFileItem->SetUserData(SvrInfo.strSvrName);
		pFileItem->SetFixedHeight(27);

		//	驱动名称
		CLabelUI* pLabelTitle = (CLabelUI*)pFileItem->FindSubControl(_T("Name"));
		pLabelTitle->SetText(SvrInfo.strSvrName);
		pLabelTitle->SetFont(pListInfo->nFont);
		pLabelTitle->SetForeColor(pListInfo->dwTextColor);
		// 如果文字过长显示...
		pLabelTitle->AppendTextStyle(DT_END_ELLIPSIS);

		// 显示名称
		CLabelUI* pDispaly = (CLabelUI*)pFileItem->FindSubControl(_T("Display"));
		pDispaly->SetText(SvrInfo.strDisplayName);
		pDispaly->SetFont(pListInfo->nFont);
		pDispaly->SetForeColor(pListInfo->dwTextColor);
		pDispaly->AppendTextStyle(DT_END_ELLIPSIS);

		//	描述
		/*CLabelUI* pDesc = (CLabelUI*)pFileItem->FindSubControl(_T("Desc"));
		pDesc->SetText(SvrInfo.strPath);
		pDesc->SetFont(pListInfo->nFont);
		pDesc->SetForeColor(pListInfo->dwTextColor);
		pDesc->SetTextStyle(dwTextStyle);*/

		CDuiString strDriverInfo;

		// 设备类型
		CLabelUI* pDriverType = (CLabelUI*)pFileItem->FindSubControl(_T("DriverType"));
		CCoreDriver::FormatDriverType(SvrInfo.dwType, strDriverInfo);
		pDriverType->SetText(strDriverInfo);
		pDriverType->AppendTextStyle(DT_END_ELLIPSIS);
		pDriverType->SetForeColor(pListInfo->dwTextColor);
		pDriverType->SetFont(pListInfo->nFont);

		// 运行类型
		CLabelUI* pRunStatus = (CLabelUI*)pFileItem->FindSubControl(_T("RunStatus"));
		CCoreDriver::FormatRunStatus(SvrInfo.dwRunStatus, strDriverInfo);
		pRunStatus->SetText(strDriverInfo);
		pRunStatus->AppendTextStyle(DT_END_ELLIPSIS);
		pRunStatus->SetForeColor(pListInfo->dwTextColor);
		pRunStatus->SetFont(pListInfo->nFont);

		// 启动类型
		CLabelUI* pStartType = (CLabelUI*)pFileItem->FindSubControl(_T("StartType"));
		CCoreDriver::FormatStartType(SvrInfo.dwStartType, strDriverInfo);
		pStartType->SetText(strDriverInfo);
		pStartType->AppendTextStyle(DT_END_ELLIPSIS);
		pStartType->SetForeColor(pListInfo->dwTextColor);
		pStartType->SetFont(pListInfo->nFont);
	}
}

void CCoreDriver::OnItemActive(TNotifyUI& msg)
{
	CDuiString strDrvName = msg.pSender->GetUserData();
}

BOOL CCoreDriver::FormatDriverType(DWORD dwDriverType, CDuiString& strDriverType)
{
	strDriverType.Empty();
	switch(dwDriverType)
	{
		case SERVICE_FILE_SYSTEM_DRIVER:
				strDriverType = _T("文件系统驱动");
			break;
		case SERVICE_KERNEL_DRIVER:
				strDriverType = _T("设备驱动");
			break;
		case SERVICE_WIN32_OWN_PROCESS:
				strDriverType = _T("独立进程服务");
			break;
		case SERVICE_WIN32_SHARE_PROCESS:
				strDriverType = _T("共享进程服务");
			break;
	}
	return strDriverType.IsEmpty() == FALSE;
}

BOOL CCoreDriver::FormatRunStatus(DWORD dwRunStatus, CDuiString& strRunStatus)
{
	strRunStatus.Empty();
	switch(dwRunStatus)
	{
		case SERVICE_RUNNING:
				strRunStatus = _T("已启动");
			break;
		case SERVICE_PAUSED:
				strRunStatus = _T("暂停");
			break;
		case SERVICE_PAUSE_PENDING:
				strRunStatus = _T("暂停中");
			break;
		case SERVICE_START_PENDING:
				strRunStatus = _T("启动中");
			break;
		case SERVICE_STOP_PENDING:
				strRunStatus = _T("停止中");
			break;
		case SERVICE_CONTINUE_PENDING:
				strRunStatus = _T("恢复中");
			break;
	}

	return strRunStatus.IsEmpty() == FALSE;
}

BOOL CCoreDriver::FormatStartType(DWORD dwStartType, CDuiString& strStartType)
{
	strStartType.Empty();
	switch(dwStartType)
	{
		case SERVICE_SYSTEM_START:
				strStartType = _T("系统启动");
			break;
		case SERVICE_DISABLED:
				strStartType = _T("禁止启动");
			break;
		case SERVICE_DEMAND_START:
				strStartType = _T("手动启动");
			break;
		case SERVICE_BOOT_START:
				strStartType = _T("引导启动");
			break;
		case SERVICE_AUTO_START:
				strStartType = _T("自动启动");
			break;
	}

	return strStartType.IsEmpty() == FALSE;
}

BOOL CCoreDriver::GetDriverInfo(CSrvInfoList& SvrInfoList)
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

		if ( EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_DRIVER, SERVICE_STATE_ALL, NULL, 0, 
			&dwBytesNeeded, &dwReslut, &dwResume, NULL)  == FALSE)
		{		
			lpEss = (LPENUM_SERVICE_STATUS_PROCESS) new BYTE[dwBytesNeeded];
			if (lpEss == NULL)
				break;
		}
		// 枚举服务，得到服务的进程ID，服务名
		EnumServicesStatusEx(hSCMag, SC_ENUM_PROCESS_INFO, SERVICE_DRIVER, SERVICE_STATE_ALL, (LPBYTE)lpEss, dwBytesNeeded, 
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
			/* SvrInfo.strPath = CString (lpSC->lpBinaryPathName);*/
			SvrInfo.strDescription = lpsd->lpDescription;

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