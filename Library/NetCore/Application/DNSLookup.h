#pragma once

#ifdef LS_STATIC_LIB_CALL
#define NET_API 
#elif defined  NETCORE_EXPORTS
#define NET_API __declspec(dllexport)
#else
#define NET_API __declspec(dllimport)
#endif

#include <WinSock2.h>
#include "DllCore/Utils/StdPtr.h"

#define MAX_DOMAINNAME_LEN  255
#define DNS_PORT								53
#define DNS_TYPE_SIZE						2
#define DNS_CLASS_SIZE					2
#define DNS_TTL_SIZE						4
#define DNS_DATALEN_SIZE				2
#define DNS_TYPE_A							0x0001 //1 a host address
#define DNS_TYPE_CNAME				0x0005 //5 the canonical name for an alias
#define DNS_PACKET_MAX_SIZE (sizeof(DNSHeader) + MAX_DOMAINNAME_LEN + DNS_TYPE_SIZE + DNS_CLASS_SIZE)

typedef struct tagDNSHeader
{
	USHORT usTransID; //标识符
	USHORT usFlags; //各种标志位
	USHORT usQuestionCount; //Question字段个数 
	USHORT usAnswerCount; //Answer字段个数
	USHORT usAuthorityCount; //Authority字段个数
	USHORT usAdditionalCount; //Additional字段个数
}DNSHeader;

class NET_API CDNSLookup
{
public:
	CDNSLookup();
	~CDNSLookup();

public:
	BOOL GetCNameInfo(CStdArray& strCNameArray) const;
	BOOL GetHostIPAddr(CStdArray& strHostIPAddr) const;
	void SetDNSServer(LPCTSTR lpszDNSServer);
	BOOL DNSQuery(LPCTSTR lpszDomainName, ULONG ulTimeout = 1000, ULONG *pulTimeSpent = NULL);
private:
	BOOL Init();
	BOOL UnInit();
	BOOL DNSLookupCore(ULONG ulDNSServerIP, LPCTSTR lpszDomainName, ULONG ulTimeout, ULONG *pulTimeSpent);
	BOOL SendDNSRequest(sockaddr_in sockAddrDNSServer, LPCSTR lpszDomainName);
	BOOL RecvDNSResponse(sockaddr_in sockAddrDNSServer, ULONG ulTimeout, ULONG *pulTimeSpent);
	BOOL EncodeDotStr(LPCSTR lpszDotStr, char *szEncodedStr, USHORT nEncodedStrSize);
	BOOL DecodeDotStr(char *szEncodedStr, USHORT *pusEncodedStrLen, char *szDotStr, USHORT nDotStrSize, char *szPacketStartPos = NULL);
	ULONG GetTickCountCalibrate();
	BOOL GetDefaultDNSServer(DWORD& dwDefaultDNS);
private:
	BOOL m_bIsInitOK;
	SOCKET m_DnsSock;
	WSAEVENT m_event;
	ULONG   m_uDnsServerIP;
	USHORT m_usCurrentProcID;
	CHAR*		m_szDNSPacket;
	CStdArray m_strIPArray;							// 点分10进制字符串
	CStdArray m_strCNameArray;					// 域名对应的CNAME
};