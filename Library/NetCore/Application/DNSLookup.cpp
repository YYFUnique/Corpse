#include "StdAfx.h"
#include "DNSLookup.h"
#include "DllCore/Log/LogHelper.h"

CDNSLookup::CDNSLookup()
	: m_bIsInitOK(FALSE)
	, m_DnsSock(INVALID_SOCKET)
	, m_szDNSPacket(NULL)
	, m_uDnsServerIP(0)
{
    m_bIsInitOK = Init();
}

CDNSLookup::~CDNSLookup()
{
    UnInit();
}

void CDNSLookup::SetDNSServer(LPCTSTR lpszDNSServer)
{
	m_uDnsServerIP = inet_addr(CStringA(lpszDNSServer));
}

BOOL CDNSLookup::GetCNameInfo(CStdArray& strCNameArray) const
{
	strCNameArray.Append(m_strCNameArray);
	return strCNameArray.IsEmpty() == FALSE;
}

BOOL CDNSLookup::GetHostIPAddr(CStdArray& strHostIPAddr) const
{
	strHostIPAddr.Append(m_strIPArray);
	return strHostIPAddr.IsEmpty() == FALSE;
}

BOOL CDNSLookup::DNSQuery(LPCTSTR lpszDomainName, ULONG ulTimeout, ULONG *pulTimeSpent)
{
    return DNSLookupCore(m_uDnsServerIP, lpszDomainName, ulTimeout, pulTimeSpent);

	//m_veculIPList.
 //   if (bRet)
 //   {
 //       char szIP[16] = {'\0'};
 //       for (std::vector<ULONG>::iterator iter = pveculIPList->begin(); iter != pveculIPList->end(); ++iter)
 //       {
 //           BYTE *pbyIPSegment = (BYTE*)(&(*iter));
 //           //sprintf_s(szIP, 16, "%d.%d.%d.%d", pbyIPSegment[0], pbyIPSegment[1], pbyIPSegment[2], pbyIPSegment[3]);
 //           sprintf_s(szIP,_countof(szIP), "%d.%d.%d.%d", pbyIPSegment[0], pbyIPSegment[1], pbyIPSegment[2], pbyIPSegment[3]);
 //           pvecstrIPList->push_back(szIP);
 //       }
 //   }
}

BOOL CDNSLookup::Init()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR)
        return FALSE;
    
    if ((m_DnsSock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
        return FALSE;

    m_event = WSACreateEvent();
    WSAEventSelect(m_DnsSock, m_event, FD_READ);

    m_szDNSPacket = new (std::nothrow) char[DNS_PACKET_MAX_SIZE];
    if (m_szDNSPacket == NULL)
        return FALSE;

    m_usCurrentProcID = (USHORT)GetCurrentProcessId();
    return TRUE;
}

BOOL CDNSLookup::UnInit()
{
    WSACleanup();

    if (m_szDNSPacket != NULL)
        delete [] m_szDNSPacket;

    return TRUE;
}

BOOL CDNSLookup::DNSLookupCore(ULONG ulDNSServerIP, LPCTSTR lpszDomainName, ULONG ulTimeout, ULONG *pulTimeSpent)
{
    if (m_bIsInitOK == FALSE || lpszDomainName == NULL)
        return FALSE;

	// 如果没有设置DNS，使用系统默认DNS服务器
	if (ulDNSServerIP == 0)
		GetDefaultDNSServer(ulDNSServerIP);

	if (ulDNSServerIP == 0)
		return FALSE;

    // 配置SOCKET
    SOCKADDR_IN SockAddr; 
    SockAddr.sin_family = AF_INET; 
    SockAddr.sin_addr.s_addr = ulDNSServerIP;
    SockAddr.sin_port = htons(DNS_PORT);

    // DNS查询与解析
    if (SendDNSRequest(SockAddr, CStringA(lpszDomainName)) == FALSE)
		return FALSE;
    if (RecvDNSResponse(SockAddr, ulTimeout, pulTimeSpent) == FALSE)
        return FALSE;
    return TRUE;
}

BOOL CDNSLookup::SendDNSRequest(SOCKADDR_IN sockAddrDNSServer, LPCSTR lpszDomainName)
{
    CHAR *pWriteDNSPacket = (CHAR*)m_szDNSPacket;
    ZeroMemory(pWriteDNSPacket, DNS_PACKET_MAX_SIZE);    

    //填充DNS查询报文头部
    DNSHeader *pDNSHeader = (DNSHeader*)pWriteDNSPacket;
    pDNSHeader->usTransID = m_usCurrentProcID;
    pDNSHeader->usFlags = htons(0x0100);
    pDNSHeader->usQuestionCount = htons(0x0001);
    pDNSHeader->usAnswerCount = 0x0000;
    pDNSHeader->usAuthorityCount = 0x0000;
    pDNSHeader->usAdditionalCount = 0x0000;

    //设置DNS查询报文内容
    USHORT usQType = htons(0x0001);
    USHORT usQClass = htons(0x0001);
    USHORT nDomainNameLen = strlen(lpszDomainName);
    CHAR *szEncodedDomainName = (CHAR *)malloc(nDomainNameLen + 2);
    if (szEncodedDomainName == NULL)
        return FALSE;

    if (EncodeDotStr(lpszDomainName, szEncodedDomainName, nDomainNameLen + 2) == FALSE)
        return FALSE;

    //填充DNS查询报文内容
    USHORT nEncodedDomainNameLen = strlen(szEncodedDomainName) + 1;
    memcpy(pWriteDNSPacket += sizeof(DNSHeader), szEncodedDomainName, nEncodedDomainNameLen);
    memcpy(pWriteDNSPacket += nEncodedDomainNameLen, (char*)(&usQType), DNS_TYPE_SIZE);
    memcpy(pWriteDNSPacket += DNS_TYPE_SIZE, (char*)(&usQClass), DNS_CLASS_SIZE);
    free (szEncodedDomainName);

    //发送DNS查询报文
    USHORT nDNSPacketSize = sizeof(DNSHeader) + nEncodedDomainNameLen + DNS_TYPE_SIZE + DNS_CLASS_SIZE;
	DWORD dwLen = sizeof(sockAddrDNSServer);
    if (sendto(m_DnsSock, m_szDNSPacket, nDNSPacketSize, 0, (sockaddr*)&sockAddrDNSServer, dwLen) == SOCKET_ERROR)
        return FALSE;

    return TRUE;
}

BOOL CDNSLookup::RecvDNSResponse(SOCKADDR_IN sockAddrDNSServer, ULONG ulTimeout, ULONG *pulTimeSpent)
{
    ULONG ulSendTimestamp = GetTickCountCalibrate();

	m_strIPArray.RemoveAll();
	m_strCNameArray.RemoveAll();

    CHAR szRecvbuf[1024] = {'\0'};
    CHAR szDotName[128] = {'\0'};
    USHORT nEncodedNameLen = 0;

    while (TRUE)
    {
        if (WSAWaitForMultipleEvents(1, &m_event, FALSE, 100, FALSE) != WSA_WAIT_TIMEOUT)
        {
            WSANETWORKEVENTS netEvent;
            WSAEnumNetworkEvents(m_DnsSock, m_event, &netEvent);

            if (netEvent.lNetworkEvents & FD_READ)
            {
                ULONG ulRecvTimestamp = GetTickCountCalibrate();
                int nSockaddrDestSize = sizeof(sockAddrDNSServer);

                //接收响应报文
                if (recvfrom(m_DnsSock, szRecvbuf, _countof(szRecvbuf), 0, (struct sockaddr*)&sockAddrDNSServer, &nSockaddrDestSize) != SOCKET_ERROR)
                {
                    DNSHeader *pDNSHeader = (DNSHeader*)szRecvbuf;
                    USHORT usQuestionCount = 0;
                    USHORT usAnswerCount = 0;

                    if (pDNSHeader->usTransID == m_usCurrentProcID
                        && (ntohs(pDNSHeader->usFlags) & 0xfb7f) == 0x8100 //RFC1035 4.1.1(Header section format)
                        && (usQuestionCount = ntohs(pDNSHeader->usQuestionCount)) >= 0
                        && (usAnswerCount = ntohs(pDNSHeader->usAnswerCount)) > 0)
                    {
                        CHAR* pDNSData = szRecvbuf + sizeof(DNSHeader);

                        //解析Question字段
                        for (int nIndex = 0; nIndex != usQuestionCount; ++nIndex)
                        {
                            if (!DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName)))
                            {
                                return FALSE;
                            }
                            pDNSData += (nEncodedNameLen + DNS_TYPE_SIZE + DNS_CLASS_SIZE);
                        }

                        //解析Answer字段
                        for (int nIndex = 0; nIndex != usAnswerCount; ++nIndex)
                        {
                            if (DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName), szRecvbuf) == FALSE)
                                return FALSE;

                            pDNSData += nEncodedNameLen;

                            USHORT usAnswerType = ntohs(*(USHORT*)(pDNSData));
                            USHORT usAnswerClass = ntohs(*(USHORT*)(pDNSData + DNS_TYPE_SIZE));
                            ULONG usAnswerTTL = ntohl(*(ULONG*)(pDNSData + DNS_TYPE_SIZE + DNS_CLASS_SIZE));
                            USHORT usAnswerDataLen = ntohs(*(USHORT*)(pDNSData + DNS_TYPE_SIZE + DNS_CLASS_SIZE + DNS_TTL_SIZE));
                            pDNSData += (DNS_TYPE_SIZE + DNS_CLASS_SIZE + DNS_TTL_SIZE + DNS_DATALEN_SIZE);

                            if (usAnswerType == DNS_TYPE_A)
                            {
                                ULONG ulIP = *(ULONG*)(pDNSData);
								SOCKADDR_IN SockAddr;
								SockAddr.sin_family = AF_INET;
								SockAddr.sin_addr.s_addr = ulIP;
								SockAddr.sin_port = 0;

								TCHAR szIPv4[20];
								DWORD dwLen = _countof(szIPv4);
								if (WSAAddressToString((LPSOCKADDR)&SockAddr, sizeof(SOCKADDR_IN), NULL, szIPv4, &dwLen) != SOCKET_ERROR)
									m_strIPArray.Add(szIPv4);
                            }
                            else if (usAnswerType == DNS_TYPE_CNAME)
                            {
                                if (DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName), szRecvbuf) != FALSE)
									m_strCNameArray.Add(szDotName);
                            }

                            pDNSData += (usAnswerDataLen);
                        }

                        //计算DNS查询所用时间
                        if (pulTimeSpent != NULL)
                            *pulTimeSpent = ulRecvTimestamp - ulSendTimestamp;

                        break;
                    }
                }
            }
        }

        //超时退出
        if (GetTickCountCalibrate() - ulSendTimestamp > ulTimeout)
        {
			if (pulTimeSpent != NULL)
				*pulTimeSpent = ulTimeout + 1;
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * convert "www.baidu.com" to "\x03www\x05baidu\x03com"
 * 0x0000 03 77 77 77 05 62 61 69 64 75 03 63 6f 6d 00 ff
 */
BOOL CDNSLookup::EncodeDotStr(LPCSTR lpszDotStr, CHAR *szEncodedStr, USHORT nEncodedStrSize)
{
    USHORT nDotStrLen = strlen(lpszDotStr);

    if (lpszDotStr == NULL || szEncodedStr == NULL || nEncodedStrSize < nDotStrLen + 2)
        return FALSE;

    CHAR* szDotStrCopy = new CHAR[nDotStrLen + 1];
    strcpy_s(szDotStrCopy, nDotStrLen + 1, lpszDotStr);
    //strcpy(szDotStrCopy, szDotStr);

    CHAR* pNextToken = NULL;
    CHAR* pToken = strtok_s(szDotStrCopy, ".", &pNextToken);
    //CHAR* szLabel = strtok(szDotStrCopy, ".");
    USHORT nLabelLen = 0;
    USHORT nEncodedStrLen = 0;
    while (pToken != NULL)
    {
        if ((nLabelLen = strlen(pToken)) != 0)
        {
            //sprintf_s(szEncodedStr + nEncodedStrLen, nEncodedStrSize - nEncodedStrLen, "%c%s", nLabelLen, pLabel);
            sprintf_s(szEncodedStr + nEncodedStrLen, nEncodedStrSize - nEncodedStrLen, "%c%s", nLabelLen, pToken);
            nEncodedStrLen += (nLabelLen + 1);
        }
        //pLabel = strtok_s(NULL, ".", &pNextToken);
        pToken = strtok_s(NULL, ".", &pNextToken);
    }

    delete [] szDotStrCopy;
    
    return TRUE;
}

/*
 * convert "\x03www\x05baidu\x03com\x00" to "www.baidu.com"
 * 0x0000 03 77 77 77 05 62 61 69 64 75 03 63 6f 6d 00 ff
 * convert "\x03www\x05baidu\xc0\x13" to "www.baidu.com"
 * 0x0000 03 77 77 77 05 62 61 69 64 75 c0 13 ff ff ff ff
 * 0x0010 ff ff ff 03 63 6f 6d 00 ff ff ff ff ff ff ff ff
 */
BOOL CDNSLookup::DecodeDotStr(char *szEncodedStr, USHORT *pusEncodedStrLen, char *szDotStr, USHORT nDotStrSize, char *szPacketStartPos)
{
    if (szEncodedStr == NULL || pusEncodedStrLen == NULL || szDotStr == NULL)
        return FALSE;

    char *pDecodePos = szEncodedStr;
    USHORT usPlainStrLen = 0;
    BYTE nLabelDataLen = 0;    
    *pusEncodedStrLen = 0;

    while ((nLabelDataLen = *pDecodePos) != 0x00)
    {
        if ((nLabelDataLen & 0xc0) == 0) //普通格式，LabelDataLen + Label
        {
            if (usPlainStrLen + nLabelDataLen + 1 > nDotStrSize)
            {
                return FALSE;
            }
            memcpy(szDotStr + usPlainStrLen, pDecodePos + 1, nLabelDataLen);
            memcpy(szDotStr + usPlainStrLen + nLabelDataLen, ".", 1);
            pDecodePos += (nLabelDataLen + 1);
            usPlainStrLen += (nLabelDataLen + 1);
            *pusEncodedStrLen += (nLabelDataLen + 1);
        }
        else //消息压缩格式，11000000 00000000，两个字节，前2位为跳转标志，后14位为跳转的偏移
        {
            if (szPacketStartPos == NULL)
            {
                return FALSE;
            }
            USHORT usJumpPos = ntohs(*(USHORT*)(pDecodePos)) & 0x3fff;
            USHORT nEncodeStrLen = 0;
            if (!DecodeDotStr(szPacketStartPos + usJumpPos, &nEncodeStrLen, szDotStr + usPlainStrLen, nDotStrSize - usPlainStrLen, szPacketStartPos))
            {
                return FALSE;
            }
            else
            {
                *pusEncodedStrLen += 2;
                return TRUE;
            }
        }
    }

    szDotStr[usPlainStrLen - 1] = '\0';
    *pusEncodedStrLen += 1;

    return TRUE;
}

ULONG CDNSLookup::GetTickCountCalibrate()
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
    {
        s_ulFirstCallTick = GetTickCount();
    }
    if (s_ullFirstCallTickMS == 0)
    {
        s_ullFirstCallTickMS = llCurrentTimeMS;
    }

    return s_ulFirstCallTick + (ULONG)(llCurrentTimeMS - s_ullFirstCallTickMS);
}

BOOL CDNSLookup::GetDefaultDNSServer(DWORD& dwDefaultDNS)
{
	BOOL bSuccess = FALSE;
	FIXED_INFO *pFixedInfo = NULL;
	DWORD dwLen = 0;
	do 
	{
		DWORD dwRet = GetNetworkParams(pFixedInfo, &dwLen);
		if (dwRet == ERROR_BUFFER_OVERFLOW)
		{
			pFixedInfo = (FIXED_INFO*)malloc(dwLen);
			if (pFixedInfo == NULL)
				break;
			dwRet = GetNetworkParams(pFixedInfo, &dwLen);
		}

		if (dwRet != NO_ERROR)
			break;
		
		SOCKADDR_IN AddrIn;
		ZeroMemory(&AddrIn, sizeof(SOCKADDR_IN));
		int dwLen = sizeof(SOCKADDR_IN);

		CString strDnsIPAddr = pFixedInfo->DnsServerList.IpAddress.String;
		if (SOCKET_ERROR == WSAStringToAddress((LPTSTR)(LPCTSTR)strDnsIPAddr, AF_INET, NULL, (SOCKADDR*)&AddrIn, &dwLen))
			return FALSE;

		dwDefaultDNS = AddrIn.sin_addr.s_addr;
		bSuccess = TRUE;
	} while (FALSE);
		
	if (pFixedInfo)
		free(pFixedInfo);

	return bSuccess;
}