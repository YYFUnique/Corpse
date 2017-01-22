#pragma once
#include <Iphlpapi.h>
#include <NetCon.h>
#include <locale>
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Rpcrt4.lib")//GUID
//启用、禁用网卡
#pragma comment(lib,"ole32.lib")//CoCreateInstance函数需要

typedef CDuiList<MIB_IPFORWARDROW,const MIB_IPFORWARDROW&> CMibIpforwardIpv4List;
typedef CDuiList<NETCON_PROPERTIES,NETCON_PROPERTIES&> CNetPropertiesList;
typedef CDuiList<IP_ADAPTER_INFO,IP_ADAPTER_INFO&> CIpAdapterInfoList;
typedef CDuiList<MIB_IPNETROW,const MIB_IPNETROW&>CMibIpNetRowList;

BOOL GetAllShowAdapter(CNetPropertiesList& NetPropertiesInfo);

BOOL GetIpForwardTableIpv4(CMibIpforwardIpv4List& MibIpforwardIpv4List);

BOOL GetIpNetTable(CMibIpNetRowList& MibIpNetRowList);

BOOL GetIPAddrTable(MIB_IPADDRTABLE* &pMibIPAddrTable);

BOOL GetAdaptersInfo(CIpAdapterInfoList& AdapterList);

BOOL LsIpv4StringToAddress(LPCTSTR lpszIpv4String, SOCKADDR_IN *pAddrIn);

BOOL LsIpv4AddressToString(DWORD dwIPAddr, CDuiString& strIpv4String);