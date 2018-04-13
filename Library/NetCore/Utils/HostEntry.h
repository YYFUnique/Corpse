#pragma once

#include <vector>

namespace NetCore
{
	typedef std::vector<LPCTSTR> AliasList;
	typedef std::vector<IPAddress> AddressList;
	class NET_API HostEntry
	{
	public:
		HostEntry();
		HostEntry(const hostent* pEntry);

		HostEntry(ADDRINFO* pAddrInfo);

		HostEntry(const HostEntry& hostEntry);

		HostEntry& operator = (const HostEntry& hostEntry);

		~HostEntry();
	public:
		void Swap(HostEntry& hostEntry);

		LPCTSTR GetHostName() const;
		const AliasList& GetAliases() const;

		const AddressList& GetAddress() const;
	private:
		CString		m_strHostName;
		AliasList	m_Aliases;
		AddressList m_Address;
	};
}