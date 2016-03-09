#include "stdafx.h"
#include "AdapterInfo.h"

BOOL GetAllShowAdapter(CNetPropertiesList& NetPropertiesInfo)
{
	BOOL bSuccess = FALSE;

	INetConnectionManager *pManager=NULL;
	INetConnection *pConnection=NULL;
	IEnumNetConnection *pEnum=NULL;
	INetSharingManager *pNetSharingManager=NULL;
	INetConnectionProps *pProps=NULL;
	INetSharingConfiguration *pConfiguration=NULL;
	NETCON_PROPERTIES*   Nproperties=NULL;

	do 
	{
		//创建一个com对象
		HRESULT hRet = CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&pManager);
		if (FAILED(hRet))
			break;

		pManager->EnumConnections(NCME_DEFAULT, &pEnum);//开始枚举网卡
		ULONG           celt;
	    while(  pEnum->Next(1, &pConnection, &celt) == S_OK )
		{  
			pConnection->GetProperties(&Nproperties);
			NetPropertiesInfo.AddTail(Nproperties);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if(pManager) pManager->Release();
	if(pConnection) pConnection->Release();
	if(pEnum) pEnum->Release();
	if(pNetSharingManager) pNetSharingManager->Release();
	if(pConfiguration) pConfiguration->Release();

	return bSuccess;
}

BOOL GetIpForwardTableIpv4(CMibIpforwardIpv4List& MibIpforwardIpv4List)
{
	BOOL bSuccess = FALSE;

	PMIB_IPFORWARDTABLE pMibIpforwardTable = NULL;

	do 
	{
		DWORD dwSize = 0;
		DWORD dwRet = GetIpForwardTable(pMibIpforwardTable,&dwSize,FALSE);
		if (dwRet == ERROR_INSUFFICIENT_BUFFER)
		{
			pMibIpforwardTable =(PMIB_IPFORWARDTABLE) new BYTE[dwSize];
			if (pMibIpforwardTable == NULL)
				break;

			dwRet = GetIpForwardTable(pMibIpforwardTable,&dwSize,FALSE);
		}

		if (dwRet == NO_ERROR)
		{
			for (DWORD dwIndex=0;dwIndex<pMibIpforwardTable->dwNumEntries;++dwIndex)
				MibIpforwardIpv4List.AddTail(pMibIpforwardTable->table[dwIndex]);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (pMibIpforwardTable != NULL)
	{
		delete[] pMibIpforwardTable;
		pMibIpforwardTable = NULL;
	}

	return bSuccess;
}

BOOL GetIpNetTable(CMibIpNetRowList& MibIpNetRowList)
{
	BOOL bSuccess = FALSE;
	BYTE bData[1024*10];
	PMIB_IPNETTABLE pMibIpNetTable = (PMIB_IPNETTABLE)bData;

	DWORD dwSize = _countof(bData);
	do 
	{
		DWORD dwRet = GetIpNetTable(pMibIpNetTable,&dwSize,FALSE);
		if (dwRet == ERROR_INSUFFICIENT_BUFFER)
		{
			pMibIpNetTable = (PMIB_IPNETTABLE)new BYTE[dwSize];
			if (pMibIpNetTable == NULL)
				break;

			dwRet = GetIpNetTable(pMibIpNetTable,&dwSize,FALSE);
		}
		if (dwRet != NO_ERROR)
			break;

		for (UINT n=0;n<pMibIpNetTable->dwNumEntries;++n)
			MibIpNetRowList.AddTail(pMibIpNetTable->table[n]);

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL GetIPAddrTable(MIB_IPADDRTABLE* &pMibIPAddrTable)
{
	BOOL bSuccess = FALSE;
	do 
	{
		pMibIPAddrTable = NULL;
		DWORD dwSize = 0;
		DWORD dwRet = GetIpAddrTable(pMibIPAddrTable, &dwSize, FALSE);
		if (dwRet == ERROR_INSUFFICIENT_BUFFER)
		{
			pMibIPAddrTable = (PMIB_IPADDRTABLE)new BYTE[dwSize];
			if (pMibIPAddrTable == NULL)
				break;
			
			dwRet = GetIpAddrTable(pMibIPAddrTable, &dwSize, TRUE);
		}

		if (dwRet != NO_ERROR)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE && pMibIPAddrTable != NULL)
	{
		delete[] pMibIPAddrTable;
		pMibIPAddrTable = NULL;
	}

	return bSuccess;
}

BOOL LsIpv4StringToAddress(LPCTSTR lpszIpv4String, SOCKADDR_IN *pAddrIn)
{
	ZeroMemory(pAddrIn,sizeof(SOCKADDR_IN));
	int dwLen = sizeof(SOCKADDR_IN);

	if (SOCKET_ERROR == WSAStringToAddress((LPTSTR)lpszIpv4String,AF_INET,NULL,(LPSOCKADDR)pAddrIn,&dwLen))
		return FALSE;

	return TRUE;
}

BOOL LsIpv4AddressToString(DWORD dwIPAddr, CDuiString& strIpv4String) 
{
	TCHAR szIpv4String[20] = {0};
	DWORD dwSize = _countof(szIpv4String);

	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = dwIPAddr;
	SockAddr.sin_port = 0;
	if (WSAAddressToString((LPSOCKADDR)&SockAddr, sizeof(SOCKADDR_IN), NULL, szIpv4String, &dwSize) == SOCKET_ERROR)
		return FALSE;

	strIpv4String = szIpv4String;

	return TRUE;
}