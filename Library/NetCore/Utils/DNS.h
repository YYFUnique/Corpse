#pragma once

namespace NetCore
{
	typedef enum tagDNS_HINT
	{
		DNS_HINT_NONE = 0,
	}DNS_HINT;

	class NET_API DNS
	{
	public:
		static HostEntry GetHostByName(LPCTSTR lpszHostName, DNS_HINT DnsHint = DNS_HINT_NONE);
	};
}