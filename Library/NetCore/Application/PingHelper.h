#pragma once

#ifdef LS_STATIC_LIB_CALL
#define NET_API 
#elif defined  NETCORE_EXPORTS
#define NET_API __declspec(dllexport)
#else
#define NET_API __declspec(dllimport)
#endif

#include <WinSock2.h>

#define DEF_PACKET_SIZE 32
#define ECHO_REQUEST		8
#define ECHO_REPLY			0

typedef struct tagIPHeader
{
	BYTE m_byVerHLen;					//4位版本+4位首部长度
	BYTE m_byTOS;							//服务类型
	USHORT m_usTotalLen;				//总长度
	USHORT m_usID;						//标识
	USHORT m_usFlagFragOffset;	//3位标志+13位片偏移
	BYTE m_byTTL;							//TTL
	BYTE m_byProtocol;					//协议
	USHORT m_usHChecksum;			//首部检验和
	ULONG m_ulSrcIP;						//源IP地址
	ULONG m_ulDestIP;					//目的IP地址
}IPHeader;

typedef struct tagICMPHeader
{ 
	BYTE m_byType;							//类型
	BYTE m_byCode;							//代码
	USHORT m_usChecksum;			//检验和 
	USHORT m_usID;						//标识符
	USHORT m_usSeq;						//序号
	ULONG m_ulTimeStamp;			//时间戳（非标准ICMP头部）
}ICMPHeader;

typedef struct tagPING_REPLY
{
	USHORT m_usSeq;
	DWORD m_dwRoundTripTime;
	DWORD m_dwBytes;
	DWORD m_dwTTL;
}PING_REPLY, *PPING_REPLY;

typedef enum tagPING_RESULT
{
	ICMP_REQUEST_SUCCESS	= 0,		// ICMP成功
	ICMP_REQUEST_FAILED		=	1,		// SOCKET标准错误，具体错误请WSAGetLastError
	ICMP_REQUEST_INITFAILD = 2,		// ICMP初始化失败
	ICMP_REQUEST_NOADDR	=	3,		// 目的地址为空
	ICMP_REQUEST_TIMEOUT	=	4,		// ICMP请求超时
	ICMP_REQUEST_xxx			= 5,
}PING_RESULT;

class NET_API CPingHelper
{
public:
	CPingHelper();
	~CPingHelper();
public:
	void SetPacketSize(DWORD dwPacketSize);
	PING_RESULT Ping(DWORD dwDestIP, PING_REPLY *pPingReply = NULL, DWORD dwTimeout = 2000);
	PING_RESULT Ping(LPCTSTR szDestIP, PING_REPLY *pPingReply = NULL, DWORD dwTimeout = 2000);
private:
	PING_RESULT PingCore(DWORD dwDestIP, PING_REPLY *pPingReply, DWORD dwTimeout);
	USHORT CalCheckSum(USHORT *pBuffer, int nSize);
	ULONG GetTickCountCalibrate();
private:
	BOOL			m_bIsInitSucc;				// 初始化是否成功
	SOCKET		m_sockRaw;					// 创建原始套接字
	WSAEVENT m_hEvent;					// 事件
	USHORT		m_usCurrentProcID;		// 进程ID
	CHAR*			m_szICMPData;			// 数据包内容
	DWORD		m_dwPacketSize;			// Ping包大小（字节）
private:
	static USHORT s_usPacketSeq;
};