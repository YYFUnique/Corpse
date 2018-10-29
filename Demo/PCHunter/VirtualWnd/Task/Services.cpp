#include "stdafx.h"
#include "Services.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Log/LogHelper.h"
#include "DllCore/Utils/Security.h"
#include "Utils/TextTools.h"

#define SERVICES_NOTIFY_NAME		_T("Windows服务管理工具")

/*#define SERVICES_CONTROL_*/
#define	SERVICES_MENU_START_SRV		_T("StartSvr")
#define	SERVICES_MENU_RESTART_SRV	_T("ReStartSvr")
#define	SERVICES_MENU_STOP_SRV			_T("StopSvr")
#define	SERVICES_MENU_STOP_PROCESS	_T("StopProcess")
#define	SERVICES_MENU_AUTO_START		_T("AutoStart")
#define	SERVICES_MENU_DEMAND			_T("DemandStart")
#define	SERVICES_MENU_DISABLED			_T("DisabledStart")
#define	SERVICES_MENU_SVRNAME			_T("ServiceName")
#define	SERVICES_MENU_DISPLAY				_T("DisplayName")
#define	SERVICES_MENU_FILE_PATH			_T("FilePath")
#define	SERVICES_MENU_DELETE				_T("DeleteSvr")
#define	SERVICES_MENU_RELAYON			_T("RelayOnSvr")
#define	SERVICES_MENU_LOOKFILE			_T("LookAtFile")
#define	SERVICES_MENU_LOOKREG			_T("LookAtReg")

#define	SERVICES_CONTROL_NAME			_T("Name")
#define	SERVICES_CONTROL_DISPLAY		_T("Display")
#define	SERVICES_CONTROL_PID				_T("PID")
#define	SERVICES_CONTROL_RUN				_T("RunStatus")
#define	SERVICES_CONTROL_START			_T("StartType")
#define	SERVICES_CONTROL_PATH			_T("Path")

CServices::CServices()
{
	m_pPaintManager = NULL;
	m_dwTick = GetTickCount();
	m_dwSearch = 0;
}

CServices::~CServices()
{
	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CServices, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_INPUT, OnInput)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CServices::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
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
		STRINGorID strXmlFile(SERVER_LIST_MENU);
		pMenu->Init(NULL, strXmlFile, pt,m_pPaintManager);

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

void CServices::OnServiceMenu(CControlUI* pControl)
{
	CListUI* pAppList = (CListUI*)m_pPaintManager->FindControl(_T("Service"));
	if (pAppList->GetCurSel() == -1)
		return;

	CFileListItemUI* pItem = (CFileListItemUI*)pAppList->GetItemAt(pAppList->GetCurSel());
	CDuiString strItemName = pControl->GetName();
	if (strItemName == SERVICES_MENU_START_SRV) {
		StartSelectedSrv(pItem);
	} else if (strItemName == SERVICES_MENU_RESTART_SRV) {
		ReStartSrv(pItem);
	} else if (strItemName == SERVICES_MENU_STOP_SRV) {
		StopSelectedSrv(pItem);
	} else if (strItemName == SERVICES_MENU_STOP_PROCESS) {
		StopProcessSrv(pItem);
	} else if (strItemName == SERVICES_MENU_AUTO_START) {
		ModifyStartType(pItem, AUTO_START);
	} else if (strItemName == SERVICES_MENU_DEMAND) {
		ModifyStartType(pItem, DEMAND_START);
	} else if (strItemName == SERVICES_MENU_DISABLED) {
		ModifyStartType(pItem, DISABLED_START);
	} else if (strItemName == SERVICES_MENU_SVRNAME) {
		CopySvrInfo(pItem, COPY_ITEM_INFO_SVR_NAME);
	} else if (strItemName == SERVICES_MENU_DISPLAY) {
		CopySvrInfo(pItem, COPY_ITEM_INFO_DISPNAME);
	} else if (strItemName == SERVICES_MENU_FILE_PATH) {
		CopySvrInfo(pItem, COPY_ITEM_INFO_SVR_PATH);
	} else if (strItemName == SERVICES_MENU_DELETE) {

	} else if (strItemName == SERVICES_MENU_RELAYON) {

	} else if (strItemName == SERVICES_MENU_LOOKFILE) {

	} else if (strItemName == SERVICES_MENU_LOOKREG) {

	}
}

void CServices::StartSelectedSrv(CFileListItemUI* pElement)
{
	if (pElement == NULL)
		return;

	CString strSrvName = pElement->GetSubControlText(SERVICES_CONTROL_NAME);

	if (StartServiceByName(strSrvName) == FALSE)
	{
		SetErrorTitle(_T("启动服务[%s]失败"), strSrvName);
		QLOG_APP(GetThreadErrorInfoString());
		MessageBox(m_pPaintManager->GetPaintWindow(), GetThreadErrorInfoString(), SERVICES_NOTIFY_NAME, MB_OK|MB_ICONERROR);
	}
	else
		OnRefreshItem(pElement);
}

void CServices::ReStartSrv(CFileListItemUI* pElement)
{
	if (pElement == NULL)
		return;

	CString strSrvName = pElement->GetSubControlText(SERVICES_CONTROL_NAME);
	if (RestartServiceByName(strSrvName) == FALSE)
	{
		SetErrorTitle(_T("重启服务[%s]失败"), strSrvName);
		QLOG_APP(GetThreadErrorInfoString());
		MessageBox(m_pPaintManager->GetPaintWindow(), GetThreadErrorInfoString(), SERVICES_NOTIFY_NAME, MB_OK|MB_ICONERROR);
	}
	else
		OnRefreshItem(pElement);
}

void CServices::StopSelectedSrv(CFileListItemUI* pElement)
{
	if (pElement == NULL)
		return;

	CString strSrvName = pElement->GetSubControlText(SERVICES_CONTROL_NAME);
	if (StopServiceByName(strSrvName) == FALSE)
	{
		SetErrorTitle(_T("停止服务[%s]失败"), strSrvName);
		QLOG_APP(GetThreadErrorInfoString());
		MessageBox(m_pPaintManager->GetPaintWindow(), GetThreadErrorInfoString(), SERVICES_NOTIFY_NAME, MB_OK|MB_ICONERROR);
	}
	else
		OnRefreshItem(pElement);
}

void CServices::StopProcessSrv(CFileListItemUI* pElementItem)
{
	if (pElementItem == NULL)
		return;

	CString strSrvName = pElementItem->GetSubControlText(SERVICES_CONTROL_NAME);
	if (IsServiceRunning(strSrvName) == FALSE)
		return;

	UINT uServiceProcess = _ttoi(pElementItem->GetSubControlText(SERVICES_CONTROL_PID));
	if (TerminateProcessByProcessId(uServiceProcess) == FALSE)
	{
		SetErrorTitle(_T("结束服务进程[%s]失败"), strSrvName);
		QLOG_APP(GetThreadErrorInfoString());
		MessageBox(m_pPaintManager->GetPaintWindow(), GetThreadErrorInfoString(), SERVICES_NOTIFY_NAME, MB_OK|MB_ICONERROR);
	}
	else
		OnRefreshItem(pElementItem);
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

void CServices::OnLoadItem(TNotifyUI& msg)
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Service"));
	if (pList->GetCount())
		pList->RemoveAll();

	CSrvInfoList SvrInfoList;
	if (GetSvrInfo(SvrInfoList) == FALSE)
		return;

	if (SvrInfoList.GetCount() == 0)
		return;
	
	CDuiString strFormatText;
	TListInfoUI* pListInfo = pList->GetListInfo();
	//去掉DrawText函数中&被转义为_,比如&a会显示为'下划线a'
	pList->SetItemTextStyle(pListInfo->uTextStyle & ~DT_NOPREFIX);

	POSITION pos = SvrInfoList.GetHeadPosition();
	while(pos)
	{
		const SERVICEINFO& SvrInfo	= SvrInfoList.GetNext(pos);

		CFileListItemUI* pFileItem = NULL;
		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(SERVER_LIST_ITEM_INFO, 0, &m_RootBuilder, m_pPaintManager);
		else
			pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

		pList->Add(pFileItem);
		pFileItem->SetUserData(SvrInfo.strSvrName);
		pFileItem->SetFixedHeight(27);

		// 名称
		CLabelUI* pSvrName = (CLabelUI*)pFileItem->FindSubControl(SERVICES_CONTROL_NAME);
		pSvrName->SetText(SvrInfo.strSvrName);
		pSvrName->SetFont(pListInfo->nFont);
		pSvrName->SetForeColor(pListInfo->dwTextColor);
		// 如果文字过长显示...
		pSvrName->AppendTextStyle(DT_END_ELLIPSIS);

		// 显示名称
		CLabelUI* pDisplay = (CLabelUI*)pFileItem->FindSubControl(SERVICES_CONTROL_DISPLAY);
		pDisplay->SetText(SvrInfo.strDisplayName);
		pDisplay->SetFont(pListInfo->nFont);
		pDisplay->SetForeColor(pListInfo->dwTextColor);
		// 如果文字过长显示...
		pDisplay->AppendTextStyle(DT_END_ELLIPSIS);

		// 进程ID
		CLabelUI* pPid = (CLabelUI*)pFileItem->FindSubControl(SERVICES_CONTROL_PID);
		CServices::FormatPid(SvrInfo.dwPID, strFormatText);
		pPid->SetText(strFormatText);
		pPid->SetFont(pListInfo->nFont);
		pPid->SetForeColor(pListInfo->dwTextColor);

		// 状态
		CLabelUI* pRunType = (CLabelUI*)pFileItem->FindSubControl(SERVICES_CONTROL_RUN);
		CServices::FormatRunStatus(SvrInfo.dwRunStatus, strFormatText);
		pRunType->SetText(strFormatText);
		pRunType->AppendTextStyle(DT_END_ELLIPSIS);
		pRunType->SetForeColor(pListInfo->dwTextColor);
		pRunType->SetFont(pListInfo->nFont);

		// 启动类型
		CLabelUI* pStartType = (CLabelUI*)pFileItem->FindSubControl(SERVICES_CONTROL_START);
		CServices::FormatStartType(SvrInfo.dwStartType, strFormatText);
		pStartType->SetText(strFormatText);
		pStartType->AppendTextStyle(DT_END_ELLIPSIS);
		pStartType->SetForeColor(pListInfo->dwTextColor);
		pStartType->SetFont(pListInfo->nFont);

		// 文件路径
		CLabelUI* pFilePath = (CLabelUI*)pFileItem->FindSubControl(SERVICES_CONTROL_PATH);
		pFilePath->SetText(SvrInfo.strPath);
		pFilePath->AppendTextStyle(DT_END_ELLIPSIS);
		pFilePath->SetForeColor(pListInfo->dwTextColor);
		pFilePath->SetFont(pListInfo->nFont);
		// 描述
	}
}

void CServices::OnInput(TNotifyUI& msg)
{
	DWORD dwSearchStart = 0;
	TEventUI* pInputEvent = (TEventUI*)msg.wParam;
	DWORD dwTick = GetTickCount();

	CString strInputKey(pInputEvent->chKey);

	if (dwTick - m_dwTick < 500)
	{
		OutputDebugString(_T("11111111111111"));
		m_strInput += pInputEvent->chKey;
		dwSearchStart = m_dwSearch + 1;
	}
	else
	{
		OutputDebugString(_T("222222222222222"));
		if (m_strInput.CompareNoCase(strInputKey) == 0)
			dwSearchStart = m_dwSearch + 1;
		else
			m_strInput = strInputKey;
	}
	
	m_dwTick = dwTick;
	OutputDebugString(m_strInput);
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Service"));
	BOOL bFind = FALSE;
	for (int n=dwSearchStart; n<pList->GetCount(); ++n )
	{
		CFileListItemUI* pServiceItem = (CFileListItemUI*)pList->GetItemAt(n);

		if (pServiceItem == NULL)
			continue;

		CDuiString strSvrName = pServiceItem->GetSubControlText(_T("Name"));
		if (_tcsnicmp(strSvrName, m_strInput, m_strInput.GetLength()) == 0)
		{
			pList->EnsureVisible(n);
			pList->SelectItem(n);
			m_dwSearch = n;
			bFind = TRUE;
			break;
		}
	}

	if (bFind == FALSE)
		m_dwSearch = 0;
}

void CServices::OnRefresh(TNotifyUI& msg)
{
	CSrvInfoList SvrInfoList;
	if (GetSvrInfo(SvrInfoList) == FALSE)
		return;

	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Service"));
	for (int n=0; n<pList->GetCount(); ++n )
	{
		// 获取每一项信息
		CFileListItemUI* pServiceItem = (CFileListItemUI*)pList->GetItemAt(n);

		if (pServiceItem == NULL)
			continue;

		CDuiString strServiceName = pServiceItem->GetSubControlText(SERVICES_CONTROL_NAME);
		UINT nProcessId = _ttoi(pServiceItem->GetSubControlText(SERVICES_CONTROL_PID));
		UINT uRunStatus = _ttoi(pServiceItem->GetSubControlText(SERVICES_CONTROL_RUN));
		UINT uStartType = _ttoi(pServiceItem->GetSubControlText(SERVICES_CONTROL_START));

		POSITION pos = SvrInfoList.GetHeadPosition();
		while(pos)
		{
			const SERVICEINFO& ServiceInfo = SvrInfoList.GetNext(pos);
			if (ServiceInfo.strSvrName.CompareNoCase(strServiceName) != 0)
				continue;
			
			CDuiString strFormatInfo;
			if (ServiceInfo.dwPID != nProcessId)
			{
				CServices::FormatPid(ServiceInfo.dwPID, strFormatInfo);
				pServiceItem->SetSubControlText(SERVICES_CONTROL_PID, strFormatInfo);
			}

			if (ServiceInfo.dwRunStatus != uRunStatus)
			{
				CServices::FormatRunStatus(ServiceInfo.dwRunStatus, strFormatInfo);
				pServiceItem->SetSubControlText(SERVICES_CONTROL_RUN, strFormatInfo);
			}

			if (ServiceInfo.dwStartType != uStartType)
			{
				CServices::FormatStartType(ServiceInfo.dwStartType, strFormatInfo);
				pServiceItem->SetSubControlText(SERVICES_CONTROL_START, strFormatInfo);
			}
		}
	}
}

void CServices::OnRefreshItem(CFileListItemUI* pServiceItem)
{
	CSrvInfoList SvrInfoList;
	if (GetSvrInfo(SvrInfoList) == FALSE)
		return;

	CDuiString strServiceName = pServiceItem->GetSubControlText(SERVICES_CONTROL_NAME);
	UINT nProcessId = _ttoi(pServiceItem->GetSubControlText(SERVICES_CONTROL_PID));
	UINT uRunStatus = _ttoi(pServiceItem->GetSubControlText(SERVICES_CONTROL_RUN));
	UINT uStartType = _ttoi(pServiceItem->GetSubControlText(SERVICES_CONTROL_START));

	POSITION pos = SvrInfoList.GetHeadPosition();
	while(pos)
	{
		const SERVICEINFO& ServiceInfo = SvrInfoList.GetNext(pos);
		if (ServiceInfo.strSvrName.CompareNoCase(strServiceName) != 0)
			continue;

		CDuiString strFormatInfo;
		if (ServiceInfo.dwPID != nProcessId)
		{
			CServices::FormatPid(ServiceInfo.dwPID, strFormatInfo);
			pServiceItem->SetSubControlText(SERVICES_CONTROL_PID, strFormatInfo);
		}

		if (ServiceInfo.dwRunStatus != uRunStatus)
		{
			CServices::FormatRunStatus(ServiceInfo.dwRunStatus, strFormatInfo);
			pServiceItem->SetSubControlText(SERVICES_CONTROL_RUN, strFormatInfo);
		}

		if (ServiceInfo.dwStartType != uStartType)
		{
			CServices::FormatStartType(ServiceInfo.dwStartType, strFormatInfo);
			pServiceItem->SetSubControlText(SERVICES_CONTROL_START, strFormatInfo);
		}
	}
}

void CServices::ModifyStartType(CFileListItemUI* pServiceItem, SVR_STARTTYPE StartType)
{
	if (pServiceItem == NULL)
		return;

	CDuiString strServiceName = pServiceItem->GetSubControlText(SERVICES_CONTROL_NAME);

	if (SetServiceStartTypeConfig(strServiceName, StartType) == FALSE)
	{
		SetErrorTitle(_T("设置服务[%s]启动状态失败"), strServiceName);
		QLOG_APP(GetThreadErrorInfoString());
		MessageBox(m_pPaintManager->GetPaintWindow(), GetThreadErrorInfoString(), SERVICES_NOTIFY_NAME, MB_OK|MB_ICONERROR);
	}
	else
		OnRefreshItem(pServiceItem);
}

void CServices::FormatPid(DWORD Pid, CDuiString& strPid)
{
	strPid.Empty();
	if (Pid != 0)
		strPid.Format(_T("%u"), Pid);
}

void CServices::CopySvrInfo(CFileListItemUI* pSvrItem, COPY_ITEM_INFO CopySvrInfo)
{
	if (pSvrItem == NULL)
		return;

	//获取应用程序句柄
	HWND hWnd = m_pPaintManager->GetPaintWindow();

	if (CopySvrInfo == COPY_ITEM_INFO_SVR_NAME)
		CopyDataToClipboard(CF_UNICODETEXT, hWnd, pSvrItem->GetSubControlText(SERVICES_CONTROL_NAME));
	else if (CopySvrInfo == COPY_ITEM_INFO_DISPNAME)
		CopyDataToClipboard(CF_UNICODETEXT, hWnd, pSvrItem->GetSubControlText(SERVICES_CONTROL_DISPLAY));
	else if (CopySvrInfo == COPY_ITEM_INFO_SVR_PATH)
		CopyDataToClipboard(CF_UNICODETEXT, hWnd, pSvrItem->GetSubControlText(SERVICES_CONTROL_PATH));
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

BOOL CServices::TerminateProcessByProcessId(DWORD dwProcessId)
{
	HANDLE hProcess = NULL;
	BOOL bSuccess = FALSE;
	do 
	{
		EnablePrivilege(SE_DEBUG_NAME);
		hProcess = OpenProcess(PROCESS_TERMINATE, TRUE, dwProcessId);
		if (hProcess == NULL)
			break;

		if (TerminateProcess(hProcess, 0) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("终止进程%d失败"),dwProcessId);
			break;
		}

		Sleep(1*100);

		bSuccess = TRUE;
	} while (FALSE);

	if (hProcess != NULL)
		CloseHandle(hProcess);

	return bSuccess;
}