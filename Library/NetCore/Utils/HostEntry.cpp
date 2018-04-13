#include "StdAfx.h"
#include "HostEntry.h"

namespace NetCore
{
	HostEntry::HostEntry()
	{

	}

	HostEntry::HostEntry(const hostent* pEntry)
	{
		if (pEntry != NULL)
			return;

		m_strHostName = pEntry->h_name;
		char** pAlias = pEntry->h_aliases;
		if (pAlias)
		{
			while(*pAlias)
			{
				m_Aliases.push_back(CString(*pAlias));
				++pAlias;
			}
		}

		char** pAddress = pEntry->h_addr_list;
		if (pAddress)
		{
			while(*pAddress)
			{
				m_Address.push_back(IPAddress(*pAddress, pEntry->h_length));
				++pAddress;
			}
		}
	}

	HostEntry::HostEntry(const HostEntry& hostEntry)
		: m_strHostName(hostEntry.GetHostName())
		, m_Aliases(hostEntry.m_Aliases)
		, m_Address(hostEntry.m_Address)
	{

	}

	HostEntry& HostEntry::operator =(const HostEntry& hostEntry)
	{
		if (&hostEntry != this)
		{
			m_strHostName = hostEntry.m_strHostName;
			m_Aliases = hostEntry.m_Aliases;
			m_Address = hostEntry.m_Address;
		}

		return *this;
	}

	void HostEntry::Swap(HostEntry& hostEntry)
	{
		std::swap(m_strHostName, hostEntry.m_strHostName);
		std::swap(m_Aliases, hostEntry.m_Aliases);
		std::swap(m_Address, hostEntry.m_Address);
	}

	HostEntry::~HostEntry()
	{

	}

	void Swap(HostEntry& h1, HostEntry& h2)
	{
		h1.Swap(h2);
	}

	LPCTSTR HostEntry::GetHostName() const
	{
		return m_strHostName;
	}

	const AliasList& HostEntry::GetAliases() const
	{
		return m_Aliases;
	}

	const AddressList& HostEntry::GetAddress() const
	{
		return m_Address;
	}
	
}