#include "stdafx.h"
#include "NetWork.h"

BOOL GetRemoteInfo(SOCKET soClient,CString& strRemoteIp,WORD& wPort)
{
	ASSERT(soClient);

	SOCKADDR_IN SockAddr;
	int nSize = sizeof(SockAddr);
	getsockname(soClient,(PSOCKADDR)&SockAddr,&nSize);

	PCHAR pIP = inet_ntoa(SockAddr.sin_addr);

	wPort = SockAddr.sin_port;
	strRemoteIp = pIP;

	return TRUE;
}