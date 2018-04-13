#include "StdAfx.h"
#include "DNS.h"

namespace NetCore
{
	HostEntry DNS::GetHostByName(LPCTSTR lpszHostName, DNS_HINT DnsHint /*= DNS_HINT_NONE*/)
	{
		hostent* phe = ::gethostbyname(CStringA(lpszHostName));
		if (phe)
			return HostEntry(phe);
		return HostEntry();
	}
}