#include "StdAfx.h"
#include "PingHelper.h"
#include "DllCore/Log/LogHelper.h"
#pragma comment(lib, "Ws2_32.lib")

USHORT CPingHelper::s_usPacketSeq = 0;

CPingHelper::CPingHelper() 
	: m_szICMPData(NULL)
	, m_bIsInitSucc(FALSE)
	, m_dwPacketSize(DEF_PACKET_SIZE)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	m_hEvent = WSACreateEvent();
	m_usCurrentProcID = (USHORT)GetCurrentProcessId();

	m_sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, 0);

	if (m_sockRaw != INVALID_SOCKET)
	{
		m_bIsInitSucc = TRUE;

		int timeout = 1000;   //设置接收超时时间
	    setsockopt(m_sockRaw,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout, sizeof(timeout)); //RECVTIMEO是接收超时时

		timeout = 1000;   //设置发送超时时间
		setsockopt(m_sockRaw,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout, sizeof(timeout)); //SNDTIMEO是发送超时时间 

		m_szICMPData = (CHAR*)malloc(DEF_PACKET_SIZE + sizeof(ICMPHeader));
		if (m_szICMPData == NULL)
			m_bIsInitSucc = FALSE;
	}
}

CPingHelper::~CPingHelper()
{
	if (m_sockRaw != INVALID_SOCKET)
		closesocket(m_sockRaw);
	WSACleanup();

	if (NULL != m_szICMPData)
	{
		free(m_szICMPData);
		m_szICMPData = NULL;
	}
}

void CPingHelper::SetPacketSize(DWORD dwPacketSize)
{
	m_dwPacketSize = dwPacketSize;

	if (m_szICMPData == NULL)
		m_szICMPData = (CHAR*)malloc(m_dwPacketSize + sizeof(ICMPHeader));
	else
		m_szICMPData = (CHAR*)realloc(m_szICMPData, m_dwPacketSize + sizeof(ICMPHeader));

	if (m_szICMPData == NULL)
		m_bIsInitSucc = FALSE;	
}

PING_RESULT CPingHelper::Ping(DWORD dwDestIP, PING_REPLY *pPingReply, DWORD dwTimeout)
{  
	return PingCore(dwDestIP, pPingReply, dwTimeout);
}

PING_RESULT CPingHelper::Ping(LPCTSTR lpszDestIP, PING_REPLY *pPingReply, DWORD dwTimeout)
{  
	if (lpszDestIP != NULL)
		return PingCore(inet_addr(CStringA(lpszDestIP)), pPingReply, dwTimeout);

	return ICMP_REQUEST_NOADDR;    
}

PING_RESULT CPingHelper::PingCore(DWORD dwDestIP, PING_REPLY *pPingReply, DWORD dwTimeout)
{
	if (dwDestIP == 0)
		return ICMP_REQUEST_NOADDR; 

	//判断初始化是否成功
	if (m_bIsInitSucc == FALSE)
		return ICMP_REQUEST_INITFAILD;

	/*MIB_IPFORWARDROW MibIPForward;
	if (GetBestRoute(dwDestIP, 0, &MibIPForward) != NO_ERROR)
	{
		LOG_PRINT(_T("无路由"));
		return ICMP_REQUEST_xxx;
	}*/

	//配置SOCKET
	SOCKADDR_IN SockAddr; 
	SockAddr.sin_family = AF_INET; 
	SockAddr.sin_addr.s_addr = dwDestIP;
	int nSockaddrDestSize = sizeof(SockAddr);

	// 构建ICMP包
	int nICMPDataSize = m_dwPacketSize + sizeof(ICMPHeader);
	ULONG ulSendTimestamp = GetTickCountCalibrate();
	USHORT usSeq = ++s_usPacketSeq; 
	ZeroMemory(m_szICMPData, nICMPDataSize);
	ICMPHeader *pICMPHeader = (ICMPHeader*)m_szICMPData;
	pICMPHeader->m_byType = ECHO_REQUEST; 
	pICMPHeader->m_byCode = 0; 
	pICMPHeader->m_usID = m_usCurrentProcID;    
	pICMPHeader->m_usSeq = usSeq;
	pICMPHeader->m_ulTimeStamp = ulSendTimestamp;
	pICMPHeader->m_usChecksum = CalCheckSum((USHORT*)m_szICMPData, nICMPDataSize);

	// 发送ICMP报文
	int nRet = sendto(m_sockRaw, m_szICMPData, nICMPDataSize, 0, (SOCKADDR*)&SockAddr, nSockaddrDestSize);
	if (nRet<0)
	{
		if (WSAGetLastError()==WSAEMSGSIZE)
			SetLastError(IP_PACKET_TOO_BIG);
		else {
			switch (WSAGetLastError()) {
			case WSAENETUNREACH:
				SetLastError(IP_DEST_NET_UNREACHABLE);
				break;
			case WSAEHOSTUNREACH:
				SetLastError(IP_DEST_HOST_UNREACHABLE);
				break;
			default:
				LOG_PRINT(_T("unknown error: errno=%d\n"),WSAGetLastError());
				SetLastError(IP_GENERAL_FAILURE);
			}
		}
		return ICMP_REQUEST_SUCCESS;
	}

	/*int nRet = sendto(m_sockRaw, m_szICMPData, nICMPDataSize, 0, (SOCKADDR*)&SockAddr, nSockaddrDestSize);
	if (nRet == SOCKET_ERROR)
	{
		LOG_PRINT(_T("sendto error %d"), WSAGetLastError());
		return ICMP_REQUEST_FAILED;
	}*/
	
	/*LOG_PRINT(_T("%d sendto"), nRet);*/

	// 判断是否需要接收相应报文
	if (pPingReply == NULL)
		return ICMP_REQUEST_SUCCESS;

	CHAR szRecvbuf[256] = {"\0"};
	ulSendTimestamp = GetTickCountCalibrate();

	while (TRUE)
	{
		// 接收响应报文
		fd_set fd;
		FD_ZERO(&fd);
        FD_SET(m_sockRaw, &fd);
        timeval tv = {5, 0};
        int nResult = select(0, &fd, NULL, NULL, &tv);
		if (nResult == 0)
			return ICMP_REQUEST_TIMEOUT;

		// 超时
		if (GetTickCountCalibrate() - ulSendTimestamp >= dwTimeout)
			return ICMP_REQUEST_TIMEOUT;

		ULONG nRecvTimestamp = GetTickCountCalibrate();
		int nPacketSize = recvfrom(m_sockRaw, szRecvbuf, _countof(szRecvbuf), 0, (SOCKADDR*)&SockAddr, &nSockaddrDestSize);
		if (nPacketSize != SOCKET_ERROR)
		{
			IPHeader *pIPHeader = (IPHeader*)szRecvbuf;
			USHORT usIPHeaderLen = (USHORT)((pIPHeader->m_byVerHLen & 0x0F) * 4);
			ICMPHeader *pICMPHeader = (ICMPHeader*)(szRecvbuf + usIPHeaderLen);

			if (pICMPHeader->m_usID == m_usCurrentProcID &&	// 是当前进程发出的报文
				pICMPHeader->m_byType == ECHO_REPLY &&			// 是ICMP响应报文
				pICMPHeader->m_usSeq == usSeq )							// 是本次请求报文的响应报文
			{
				pPingReply->m_usSeq = usSeq;
				pPingReply->m_dwRoundTripTime = nRecvTimestamp - pICMPHeader->m_ulTimeStamp;
				pPingReply->m_dwBytes = nPacketSize - usIPHeaderLen - sizeof(ICMPHeader);
				pPingReply->m_dwTTL = pIPHeader->m_byTTL;
				return ICMP_REQUEST_SUCCESS;
			}
			else
			{
				LOG_PRINT(_T("pICMPHeader->m_byType:%d, pICMPHeader->m_byCode:%d.\r\n"), pICMPHeader->m_byType, pICMPHeader->m_byCode);
				return ICMP_REQUEST_xxx;
			}
		}
	}
}

USHORT CPingHelper::CalCheckSum(USHORT *pBuffer, int nSize)
{
	unsigned long ulCheckSum=0; 
	while(nSize > 1) 
	{ 
		ulCheckSum += *pBuffer++; 
		nSize -= sizeof(USHORT); 
	}
	if (nSize)
		ulCheckSum += *(UCHAR*)pBuffer; 

	ulCheckSum = (ulCheckSum >> 16) + (ulCheckSum & 0xFFFF); 
	ulCheckSum += (ulCheckSum >>16); 

	return (USHORT)(~ulCheckSum); 
}

ULONG CPingHelper::GetTickCountCalibrate()
{
	static ULONG s_ulFirstCallTick = 0;
	static LONGLONG s_ullFirstCallTickMS = 0;

	SYSTEMTIME systemtime;
	FILETIME filetime;
	GetLocalTime(&systemtime);    
	SystemTimeToFileTime(&systemtime, &filetime);

	LARGE_INTEGER liCurrentTime;
	liCurrentTime.HighPart = filetime.dwHighDateTime;
	liCurrentTime.LowPart = filetime.dwLowDateTime;
	LONGLONG llCurrentTimeMS = liCurrentTime.QuadPart / 10000;

	if (s_ulFirstCallTick == 0)
		s_ulFirstCallTick = GetTickCount();
	if (s_ullFirstCallTickMS == 0)
		s_ullFirstCallTickMS = llCurrentTimeMS;

	return s_ulFirstCallTick + (ULONG)(llCurrentTimeMS - s_ullFirstCallTickMS);
}