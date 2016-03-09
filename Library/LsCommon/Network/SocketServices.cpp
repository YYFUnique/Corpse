#include "StdAfx.h"
#include "SocketServices.h"
#include "../ErrorInfo.h"
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

CSocketServices::CSocketServices()
{
	m_lpCompletionRoutine = NULL;
	m_NotificationHandle = NULL;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);

	memset(&m_OverLapped,0,sizeof(WSAOVERLAPPED));

	WSAProviderConfigChange(&m_NotificationHandle,NULL,NULL);
}

CSocketServices::~CSocketServices()
{
	if (m_NotificationHandle != NULL)
		CloseHandle(m_NotificationHandle);

	WSACleanup();
}

BOOL CSocketServices::LsEnumProtocols(CWscProtocolInfoList& WscProtocolInfo)
{
	BOOL bSuccess = FALSE;
	BYTE ByteData[1024*32];
	LPWSAPROTOCOL_INFOW lpWsaProtocolInfo = (LPWSAPROTOCOL_INFOW)ByteData;
	DWORD dwSize = _countof(ByteData);
	int nErrorCode = ERROR_SUCCESS;
	do 
	{
		int nCount = WSCEnumProtocols(NULL,lpWsaProtocolInfo,&dwSize,&nErrorCode);
		if (nErrorCode == WSAENOBUFS)
		{
			lpWsaProtocolInfo = (LPWSAPROTOCOL_INFOW)new BYTE[dwSize];
			if (lpWsaProtocolInfo == NULL)
				break;

			nCount = WSCEnumProtocols(NULL,lpWsaProtocolInfo,&dwSize,&nErrorCode);
		}

		if (nErrorCode == WSAENOBUFS)
			break;

		for (int n=0;n<nCount;++n)
			WscProtocolInfo.AddTail(lpWsaProtocolInfo[n]);

		bSuccess = TRUE;
	} while (FALSE);

	if (lpWsaProtocolInfo != NULL && lpWsaProtocolInfo != (LPWSAPROTOCOL_INFOW)ByteData)
	{
		delete[] lpWsaProtocolInfo;
		lpWsaProtocolInfo = NULL;
	}

	WSAProviderConfigChange(&m_NotificationHandle,&m_OverLapped,m_lpCompletionRoutine);
	return bSuccess;
}

BOOL CSocketServices::LsGetProviderPath(LPGUID lpGuid,CString& strProviderPath)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (lpGuid == NULL)
			break;

		WCHAR szProviderPath[MAX_PATH];
		int nSize = _countof(szProviderPath);
		int nErrorCode = ERROR_SUCCESS;
		if (WSCGetProviderPath(lpGuid,szProviderPath,&nSize,&nErrorCode) == SOCKET_ERROR)
			break;

//以下代码是将宽字节根据需要转换为需要的类型
#ifdef _UNICODE
		strProviderPath = szProviderPath;
#else
		strProviderPath.Format(_T("%ws"),szProviderPath);
#endif

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void CSocketServices::SetCompletionRoutine(LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	m_lpCompletionRoutine = lpCompletionRoutine;
}

BOOL CSocketServices::IsProviderConfigChanged()
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_NotificationHandle == NULL)
			break;

		DWORD dwRet = WaitForSingleObject(m_NotificationHandle,0);
		if (dwRet != WAIT_OBJECT_0)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}