#pragma once

#ifdef LS_STATIC_LIB_CALL
#define	LS_COMMON_API
#elif defined LS_EXPORTS
#define	LS_COMMON_API  __declspec(dllexport)
#else
#define	LS_COMMON_API  __declspec(dllimport)
#endif

#include < Iphlpapi.h>
#include <Ws2spi.h>
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Ws2_32.lib")

#define	TABLE_SIZE			1024

typedef CList<MIB_IPNETROW, MIB_IPNETROW &> CMibIpNetRowList;

typedef CList<WSAPROTOCOL_INFO,WSAPROTOCOL_INFO&> CWsaProtocolInfoList;

typedef CList<MIB_IPFORWARD_ROW2,MIB_IPFORWARD_ROW2&> CMibIpforwardTableList;
typedef CList<MIB_IPFORWARDROW,MIB_IPFORWARDROW&> CMibIpforwardIpv4List;

typedef struct _LOCAL_NET_CONNECTION_STATE
{
	DWORD   dwProtol;					//协议ID
	DWORD   dwLocalIpAddr;			//本地IP地址
	DWORD   dwLocalPort;				//本地端口
	DWORD   dwRemoteIpAddr;		//对方IP
	DWORD   dwRemotePort;			//对方端口
	DWORD   dwState;						//状态
	DWORD   dwProcessID;				//进程ID
	CString		 strProcessName;		//进程或者服务名
	CString		 strProcessAuth;			//进程所属用户组
	CString		 strLocalProcessPath;  //本地进程路径
	LARGE_INTEGER	liCreateTime;//创建时间
}LOCAL_NET_CONNECTION_STATE, *PLOCAL_NET_CONNECTION_STATE;

typedef CList<LOCAL_NET_CONNECTION_STATE, LOCAL_NET_CONNECTION_STATE&> CLocalNetConnectionStateList;

LS_COMMON_API BOOL LsGetIpNetTable(CMibIpNetRowList& MibIpNetRowList);


/************************************************************************/
// 函数名称：FlushDynamicArp
// 函数功能：清除系统的ARP缓存信息
// 函数参数：无
// 返  回 值：执行成功返回TRUE，否则返回FALSE
/************************************************************************/
LS_COMMON_API BOOL FlushDynamicArp();

/************************************************************************/
// 函数名称：GetIpString
// 函数功能：将整数类型的Ip地址转换为点分十进制IP地址
// 函数参数：整数类型的IP地址
// 返  回 值：执行成功返回对应的IP地址
/************************************************************************/
LS_COMMON_API CString GetIpString(DWORD dwIPAddr);

/************************************************************************/
// 函数名称：GetSystemNetworkProtocol
// 函数功能：获取系统的网络协议
// 函数参数：lpiProtocols 协议类型，如果为空，则获取所有的协议
//				     WsaProtocolInfoList 保存协议的链表
// 返  回 值：执行成功返回TRUE，否则返回FALSE
/************************************************************************/
LS_COMMON_API BOOL GetSystemNetworkProtocol(LPINT lpiProtocols,CWsaProtocolInfoList& WsaProtocolInfoList);

/************************************************************************/
// 函数名称：GetIpForwardTableIpv4
// 函数功能：获取系统IPv4路由状态信息
// 函数参数：MibIpforwardTableList 路由链表
// 返  回 值：执行成功返回TRUE，否则返回FALSE
/************************************************************************/
LS_COMMON_API BOOL GetIpForwardTableIpv4(CMibIpforwardIpv4List& MibIpforwardIpv4List);

/************************************************************************/
// 函数名称：GetIpForwardTableList
// 函数功能：获取系统路由状态信息
// 函数参数：Family 路由版本IPv4(AF_INET)或IPv6(AF_INET6)
//				     MibIpforwardTableList 路由链表
// 返  回 值：执行成功返回TRUE，否则返回FALSE
/************************************************************************/
LS_COMMON_API BOOL GetIpForwardTableList(CMibIpforwardTableList& MibIpforwardTableList,ADDRESS_FAMILY  Family = AF_UNSPEC);

LS_COMMON_API BOOL GetTcpNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList);

LS_COMMON_API BOOL GetUdpNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList);

LS_COMMON_API BOOL GetNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList);