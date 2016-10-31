#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSTcpIP.h>
#pragma comment(lib,"ws2_32.lib")

#define PROTOCOL_STRING_ICMP	_T("ICMP")
#define PROTOCOL_STRING_TCP		_T("TCP")
#define PROTOCOL_STRING_UDP		_T("UDP")
#define PROTOCOL_STRING_SPX		_T("SPX")
#define PROTOCOL_STRING_NCP		_T("NCP")

typedef enum PROTOCOL_TYPE
{
	PROTOCOL_TYPE_ICMP = 1,
	PROTOCOL_TYPE_IGMP = 2,
	PROTOCOL_TYPE_TCP		= 6,
	PROTOCOL_TYPE_UDP   = 17,
}PROTOCOL_TYPE;

//定义IP首部
typedef struct IP_HEADER 
{
	BYTE	bLenVer;					//版本信息(前4位)，头长度(后4位)
	BYTE	bTypeOfService;		//服务类型8位
	WORD wPacketLen;				//数据包长度
	WORD wPacketId;				//数据包标识
	WORD	wSliceInfo;				//分片使用
	BYTE	bTTL;						//存活时间
	BYTE	bProtoType;				//协议类型(TCP,UDP,ICMP...)    
	WORD	wCheckSum;			//IP首部校验和
	DWORD dwSrcIp;
	DWORD dwDstIp;
}IP_HEADER,*PIP_HEADER;

//定义TCP伪首部
//typedef struct 

//定义TCP首部
typedef struct TCP_HEADER
{

}TCP_HEADER,*PTCP_HEADER;

//定义ICMP首部
typedef struct ICMP_HEADER
{

}ICMP_HEADER,*PICMP_HEADER;

//定义UDP首部
typedef struct UDP_HEADER
{

}UDP_HEADER,*PUDP_HEADER;