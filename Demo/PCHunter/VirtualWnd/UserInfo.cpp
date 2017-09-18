#include "stdafx.h"

#pragma comment(lib,"WtsApi32.lib")

CUserInfo::CUserInfo()
{

}

CUserInfo::~CUserInfo()
{

}

void CUserInfo::OnPaint()
{
	PWTS_SESSION_INFO pWtsSessionInfo = NULL;
	DWORD dwCount = 0;

	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("User"));
	if (pList->GetCount())
		pList->RemoveAll();
	//CSvrInfo SvrInfoList;

	BOOL bRet = WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE,NULL,1,&pWtsSessionInfo,&dwCount);
	CDuiString strSessionInfo;
	for (UINT n=0;n<dwCount;++n)
	{
		WTS_SESSION_INFO WtsSessionInfo = pWtsSessionInfo[n];
		CListTextElementUI* pTextElement = new CListTextElementUI;
		pList->Add(pTextElement);


		TCHAR szType[20];
		_stprintf_s(szType,_countof(szType),_T("%d"),WtsSessionInfo.SessionId);
		pTextElement->SetText(0,szType);
		if (QuerySessionInfo(WtsSessionInfo.SessionId,WTSUserName,strSessionInfo) != FALSE)
			pTextElement->SetText(1,strSessionInfo);

		if (QuerySessionState(WtsSessionInfo.State,strSessionInfo) != FALSE)
			pTextElement->SetText(2,strSessionInfo);

 		if (QuerySessionInfo(WtsSessionInfo.SessionId,WTSClientName,strSessionInfo) != FALSE)
 			pTextElement->SetText(3,strSessionInfo);

		if (QueryClientIp(WtsSessionInfo.SessionId,strSessionInfo) != FALSE)
			pTextElement->SetText(4,strSessionInfo);

		pTextElement->SetText(5,pWtsSessionInfo[n].pWinStationName);		
	}

	if (pWtsSessionInfo != NULL)
		WTSFreeMemory(pWtsSessionInfo);
}

BOOL CUserInfo::QuerySessionInfo(DWORD dwSessionId,WTS_INFO_CLASS WTSInfoClass,CDuiString& strInfo)
{
	BOOL bSuccess = FALSE;
	LPTSTR lpData = NULL;
	DWORD dwSize = 0;
	do 
	{
		if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,dwSessionId,WTSInfoClass,&lpData,&dwSize) == FALSE)
			break;

		strInfo = lpData;

		bSuccess = TRUE;
	} while (FALSE);

	if (lpData)
		WTSFreeMemory(lpData);

	return bSuccess;
}

BOOL CUserInfo::QuerySessionState(WTS_CONNECTSTATE_CLASS WtsState,CDuiString& strState)
{
	switch(WtsState)
	{
	case WTSActive :
			strState = _T("Active");
		break;
	case WTSConnected :
			strState = _T("Connected");
		break;
	case WTSConnectQuery :
			strState = _T("ConnectQuery");
		break;
	case WTSShadow :
			strState = _T("Shadow");
		break;
	case WTSDisconnected :
			strState = _T("Disconnected");
		break;
	case WTSIdle:
			strState = _T("Idle");
		break;
	case WTSListen :
			strState = _T("Listen");
		break;
	case WTSReset :
			strState = _T("Reset");
		break;
	case WTSDown :
			strState = _T("State");
		break;
	case WTSInit:
			strState = _T("Init");
		break;
	default:
		strState = _T("Error");
	}
	return TRUE;
}


BOOL CUserInfo::QueryClientIp(DWORD dwSessionId,CDuiString& strInfo)
{
	BOOL bSuccess = FALSE;
	LPTSTR lpData = NULL;
	DWORD dwSize = 0;
	do 
	{
		if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,dwSessionId,WTSClientAddress,&lpData,&dwSize) == FALSE)
			break;

		if (lpData[0] == _T('\0'))
			break;

		PWTS_CLIENT_ADDRESS pWTSCA = (PWTS_CLIENT_ADDRESS)lpData;
		strInfo.Format(_T("%d.%d.%d.%d"),pWTSCA->Address[2],pWTSCA->Address[3],pWTSCA->Address[4],pWTSCA->Address[5]);

		bSuccess = TRUE;
	} while (FALSE);

	if (lpData)
		WTSFreeMemory(lpData);

	return bSuccess;
}