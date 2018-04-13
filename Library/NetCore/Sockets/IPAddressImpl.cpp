#include "StdAfx.h"
#include "IPAddressImpl.h"

namespace NetCore
{
	//
	// IPAddressImpl
	//
	IPAddressImpl::IPAddressImpl()
	{

	}

	IPAddressImpl::~IPAddressImpl()
	{

	}

	//
	// IPv4AddressImpl
	//

	IPv4AddressImpl::IPv4AddressImpl()
	{
		ZeroMemory(&m_Addr, sizeof(IN_ADDR));
	}

	IPv4AddressImpl::IPv4AddressImpl(const void* pAddr)
	{
		memcpy(&m_Addr, pAddr, sizeof(IN_ADDR));
	}

	//IPv4AddressImpl::IPv4AddressImpl()

	IPv4AddressImpl::IPv4AddressImpl(const IPv4AddressImpl& Addr)
	{
		memcpy(&m_Addr, Addr.GetAddr(), sizeof(IN_ADDR));
	}

	IPv4AddressImpl& IPv4AddressImpl::operator = (const IPv4AddressImpl& Addr)
	{
		if (this == &Addr)
			return *this;

		memcpy(&m_Addr, Addr.GetAddr(), sizeof(IN_ADDR));
		return *this;
	}

	LPCTSTR IPv4AddressImpl::ToString() const
	{
		const BYTE* b = reinterpret_cast<const BYTE*>(&m_Addr);
		CString strResult;
		strResult.Format(_T("%d.%d.%d.%d"),b[0],b[1],b[2],b[3]);
		return strResult;
	}

	poco_socklen_t IPv4AddressImpl::GetLen() const
	{
		return sizeof(IN_ADDR);
	}

	const void* IPv4AddressImpl::GetAddr() const
	{
		return &m_Addr;
	}

	Family IPv4AddressImpl::GetFamily() const
	{
		return AddressFamily::IPv4;
	}

	int IPv4AddressImpl::GetAF() const
	{
		return AF_INET;
	}

	UINT IPv4AddressImpl::GetScope() const
	{
		return 0;
	}

	BOOL IPv4AddressImpl::IsWildcard() const
	{
		return m_Addr.s_addr == INADDR_ANY;
	}

	BOOL IPv4AddressImpl::IsBroadcast() const
	{
		return m_Addr.s_addr == INADDR_NONE;
	}

	BOOL IPv4AddressImpl::IsLoopback() const
	{
		// 127.0.0.1 to 127.255.255.255
		return (ntohl(m_Addr.s_addr) & 0xFF000000) == 0x7F000000;
	}

	BOOL IPv4AddressImpl::IsMulticast() const
	{
		// 224.0.0.0/24 to 239.0.0.0/24
		return (ntohl(m_Addr.s_addr) & 0xF0000000) == 0xE0000000;
	}

	BOOL IPv4AddressImpl::IsLinkLocal() const
	{
		// 169.254.0.0/16
		return (ntohl(m_Addr.s_addr) & 0xFFFF0000) == 0xA9FE0000;
	}

	BOOL IPv4AddressImpl::IsSiteLocal() const
	{
		UINT nAddr = ntohl(m_Addr.s_addr);

		return (nAddr & 0xFF000000) == 0x0A000000 ||					// 10.0.0.0/24
					(nAddr & 0xFFFF0000) == 0xC0A80000 ||					// 192.68.0.0/16
					(nAddr >= 0xAC100000 && nAddr <= 0xAC1FFFFF); // 172.16.0.0 to 172.31.255.255
	}

	BOOL IPv4AddressImpl::IsIPv4Compatible() const
	{
		return TRUE;
	}

	BOOL IPv4AddressImpl::IsIPv4Mapped() const
	{
		return TRUE;
	}

	BOOL IPv4AddressImpl::IsWellKnownMC() const
	{
		return (ntohl(m_Addr.s_addr) & 0xFFFFFF00) == 0xE0000000; // 224.0.0.0/8
	}

	BOOL IPv4AddressImpl::IsNodeLocalMC() const
	{
		return FALSE;
	}

	BOOL IPv4AddressImpl::IsLinkLocalMC() const
	{
		return (ntohl(m_Addr.s_addr) & 0xFF000000) == 0xE0000000; // 244.0.0.0/24
	}

	BOOL IPv4AddressImpl::IsSiteLocalMC() const
	{
		return (ntohl(m_Addr.s_addr) & 0xFFFF0000) == 0xEFFF0000;	// 239.255.0.0/16
	}

	BOOL IPv4AddressImpl::IsOrgLocalMC() const
	{
		return (ntohl(m_Addr.s_addr) & 0xFFFF0000) == 0xEFC00000; // 239.192.0.0/16
	}

	BOOL IPv4AddressImpl::IsGlobalMC() const
	{
		UINT nAddr = ntohl(m_Addr.s_addr);
		// 224.0.1.0 to 238.255.255.255
		return nAddr >= 0xE0000100 && m_Addr.s_addr <= 0xEE000000;
	}

	IPv4AddressImpl IPv4AddressImpl::Parse(LPCTSTR lpszAddr)
	{
		if (lpszAddr == NULL || lpszAddr[0] == _T('\0'))
			return IPv4AddressImpl();

		IN_ADDR InAddr;
		InAddr.s_addr = inet_addr(CStringA(lpszAddr));
		if (InAddr.s_addr == INADDR_NONE && _tcsicmp(lpszAddr, _T("255.255.255.25")) != 0)
			return IPv4AddressImpl();
		else
			return IPv4AddressImpl(&InAddr);
	}

	int IPv4AddressImpl::GetPrefixLength() const
	{
		return 0;
	}

	void IPv4AddressImpl::Mask(const IPAddressImpl* pMask, const IPAddressImpl* pSet)
	{
		const IPv4AddressImpl* pIPv4Mask = static_cast<const IPv4AddressImpl*>(pMask);
		const IPv4AddressImpl* pIPv4Set = static_cast<const IPv4AddressImpl*>(pSet);
		m_Addr.s_addr &= pIPv4Mask->m_Addr.s_addr;
		m_Addr.s_addr |= pIPv4Set->m_Addr.s_addr & ~pIPv4Mask->m_Addr.s_addr;
	}

	IPAddressImpl* IPv4AddressImpl::Clone() const
	{
		return new IPv4AddressImpl(&m_Addr);
	}

	IPv4AddressImpl IPv4AddressImpl::operator &(const IPv4AddressImpl& Addr) const
	{
		IPv4AddressImpl Result(&m_Addr);
		Result.m_Addr.s_addr &= Addr.m_Addr.s_addr;
		return Result;
	}

	IPv4AddressImpl IPv4AddressImpl::operator |(const IPv4AddressImpl &Addr) const
	{
		IPv4AddressImpl Result(&m_Addr);
		Result.m_Addr.s_addr |= Addr.m_Addr.s_addr;
		return Result;
	}

	IPv4AddressImpl IPv4AddressImpl::operator ^(const IPv4AddressImpl& Addr) const
	{
		IPv4AddressImpl Result(&m_Addr);
		Result.m_Addr.s_addr ^= Addr.m_Addr.s_addr;
		return Result;
	}

	IPv4AddressImpl IPv4AddressImpl::operator ~() const
	{
		IPv4AddressImpl Result(&m_Addr);
		Result.m_Addr.s_addr ^= 0xFFFFFFFF;
		return Result;
	}

	BOOL IPv4AddressImpl::operator ==(const IPv4AddressImpl& Addr) const
	{
		return memcmp(&Addr.m_Addr, &m_Addr, sizeof(IN_ADDR));
	}

	BOOL IPv4AddressImpl::operator !=(const IPv4AddressImpl& Addr) const
	{
		return !(*this == Addr);
	}


	//
	// IPv6AddressImpl
	//

#ifdef POCO_HAVE_IPv6

	IPv6AddressImpl::IPv6AddressImpl()
		: m_nScope(0)
	{
		ZeroMemory(&m_Addr, sizeof(IN6_ADDR));
	}

	IPv6AddressImpl::IPv6AddressImpl(const void* pAddr)
		: m_nScope(0)
	{
		memcpy(&m_Addr, pAddr, sizeof(IN6_ADDR));
	}

	IPv6AddressImpl::IPv6AddressImpl(const void* pAddr, UINT nScope)
		: m_nScope(nScope)
	{
		memcpy(&m_Addr, pAddr, sizeof(IN6_ADDR));
	}

	IPv6AddressImpl::IPv6AddressImpl(const IPv6AddressImpl* pAddr)
		: m_nScope(0)
	{
		memcpy(&m_Addr, &pAddr->m_Addr, sizeof(IN6_ADDR));
	}

	IPv6AddressImpl::IPv6AddressImpl(const IPv6AddressImpl& Addr)
	{
		memcpy(&m_Addr, &Addr, sizeof(IN6_ADDR));
	}

	LPCTSTR IPv6AddressImpl::ToString() const
	{
		return _T("");
	}

	poco_socklen_t IPv6AddressImpl::GetLen() const
	{
		return sizeof(IN6_ADDR);
	}

	int IPv6AddressImpl::GetAF() const
	{
		return AF_INET6;
	}

	const void* IPv6AddressImpl::GetAddr() const
	{
		return &m_Addr;
	}

	Family IPv6AddressImpl::GetFamily() const
	{
		return AddressFamily::IPv6;
	}

	UINT IPv6AddressImpl::GetScope() const
	{
		return m_nScope;
	}

	BOOL IPv6AddressImpl::IsWildcard() const
	{
		const WORD* pWords = reinterpret_cast<const WORD*>(&m_Addr);
		return pWords[0] == 0 && pWords[1] == 0 && pWords[2] == 0 && pWords[3] == 0 && 
			pWords[4] == 0 && pWords[5] == 0 && pWords[6] == 0 && pWords[7] == 0;
	}

	BOOL IPv6AddressImpl::IsBroadcast() const
	{
		return FALSE;
	}

	BOOL IPv6AddressImpl::IsLoopback() const
	{
		const WORD* pWords = reinterpret_cast<const WORD*>(&m_Addr);
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 && 
			words[4] == 0 && words[5] == 0 && words[6] == 0 && ByteOrder::fromNetwork(words[7]) == 0x0001;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsMulticast() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return (ByteOrder::fromNetwork(words[0]) & 0xFFE0) == 0xFF00;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsLinkLocal() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return (ByteOrder::fromNetwork(words[0]) & 0xFFE0) == 0xFE80;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsSiteLocal() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return ((ByteOrder::fromNetwork(words[0]) & 0xFFE0) == 0xFEC0) || ((ByteOrder::fromNetwork(words[0]) & 0xFF00) == 0xFC00);*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsIPv4Compatible() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 && words[4] == 0 && words[5] == 0;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsIPv4Mapped() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 && words[4] == 0 && ByteOrder::fromNetwork(words[5]) == 0xFFFF;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsWellKnownMC() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return (ByteOrder::fromNetwork(words[0]) & 0xFFF0) == 0xFF00;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsNodeLocalMC() const 
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return (ByteOrder::fromNetwork(words[0]) & 0xFFEF) == 0xFF01;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsLinkLocalMC() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return (ByteOrder::fromNetwork(words[0]) & 0xFFEF) == 0xFF02;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsSiteLocalMC() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return (ByteOrder::fromNetwork(words[0]) & 0xFFEF) == 0xFF05;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsOrgLocalMC() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return (ByteOrder::fromNetwork(words[0]) & 0xFFEF) == 0xFF08;*/
		return TRUE;
	}

	BOOL IPv6AddressImpl::IsGlobalMC() const
	{
		/*const UInt16* words = reinterpret_cast<const UInt16*>(&_addr);
		return (ByteOrder::fromNetwork(words[0]) & 0xFFEF) == 0xFF0F;*/
		return TRUE;
	}

	IPv6AddressImpl IPv6AddressImpl::Parse(LPCTSTR lpszAddr)
	{
		//if (addr.empty()) return IPv6AddressImpl();
		if (lpszAddr == NULL || lpszAddr[0] == _T('\0'))
			return IPv6AddressImpl();
		
#if defined(_WIN32)
		struct addrinfo* pAI;
		struct addrinfo hints;
		std::memset(&hints, 0, sizeof(hints));
		hints.ai_flags = AI_NUMERICHOST;
		int rc = getaddrinfo(CStringA(lpszAddr), NULL, &hints, &pAI);
		if (rc == 0)
		{
			IPv6AddressImpl result = IPv6AddressImpl(&reinterpret_cast<struct sockaddr_in6*>(pAI->ai_addr)->sin6_addr, static_cast<int>(reinterpret_cast<struct sockaddr_in6*>(pAI->ai_addr)->sin6_scope_id));
			freeaddrinfo(pAI);
			return result;
		}
		else return IPv6AddressImpl();
#else
		struct in6_addr ia;
		std::string::size_type pos = addr.find('%');
		if (std::string::npos != pos)
		{
			std::string::size_type start = ('[' == addr[0]) ? 1 : 0;
			std::string unscopedAddr(addr, start, pos - start);
			std::string scope(addr, pos + 1, addr.size() - start - pos);
			Poco::UInt32 scopeId(0);
			if (!(scopeId = if_nametoindex(scope.c_str())))
				return IPv6AddressImpl();
			if (inet_pton(AF_INET6, unscopedAddr.c_str(), &ia) == 1)
				return IPv6AddressImpl(&ia, scopeId);
			else
				return IPv6AddressImpl();
		}
		else
		{
			if (inet_pton(AF_INET6, addr.c_str(), &ia) == 1)
				return IPv6AddressImpl(&ia);
			else
				return IPv6AddressImpl();
		}
#endif
	}

	int IPv6AddressImpl::GetPrefixLength() const
	{
		return 0;
	}

	void IPv6AddressImpl::Mask(const IPAddressImpl* pMask, const IPAddressImpl* pSet)
	{

	}

	IPAddressImpl* IPv6AddressImpl::Clone() const
	{
		return new IPv6AddressImpl(*this);
	}

	IPv6AddressImpl IPv6AddressImpl::operator &(const IPv6AddressImpl& Addr) const
	{
		IPv6AddressImpl Result(this);
		if (m_nScope != Addr.m_nScope)
			return Result;

		Result.m_Addr.u.Word[0] &= Addr.m_Addr.u.Word[0];
		Result.m_Addr.u.Word[1] &= Addr.m_Addr.u.Word[1];
		Result.m_Addr.u.Word[2] &= Addr.m_Addr.u.Word[2];
		Result.m_Addr.u.Word[3] &= Addr.m_Addr.u.Word[3];
		Result.m_Addr.u.Word[4] &= Addr.m_Addr.u.Word[4];
		Result.m_Addr.u.Word[5] &= Addr.m_Addr.u.Word[5];
		Result.m_Addr.u.Word[6] &= Addr.m_Addr.u.Word[6];
		Result.m_Addr.u.Word[7] &= Addr.m_Addr.u.Word[7];

		return Result;
	}

	IPv6AddressImpl IPv6AddressImpl::operator |(const IPv6AddressImpl& Addr) const
	{
		IPv6AddressImpl Result(this);
		if (m_nScope != Addr.m_nScope)
			return Result;

		Result.m_Addr.u.Word[0] |= Addr.m_Addr.u.Word[0];
		Result.m_Addr.u.Word[1] |= Addr.m_Addr.u.Word[1];
		Result.m_Addr.u.Word[2] |= Addr.m_Addr.u.Word[2];
		Result.m_Addr.u.Word[3] |= Addr.m_Addr.u.Word[3];
		Result.m_Addr.u.Word[4] |= Addr.m_Addr.u.Word[4];
		Result.m_Addr.u.Word[5] |= Addr.m_Addr.u.Word[5];
		Result.m_Addr.u.Word[6] |= Addr.m_Addr.u.Word[6];
		Result.m_Addr.u.Word[7] |= Addr.m_Addr.u.Word[7];

		return Result;
	}

	IPv6AddressImpl IPv6AddressImpl::operator ^(const IPv6AddressImpl& Addr) const
	{
		IPv6AddressImpl Result(this);
		if (m_nScope != Addr.m_nScope)
			return Result;

		Result.m_Addr.u.Word[0] ^= Addr.m_Addr.u.Word[0];
		Result.m_Addr.u.Word[1] ^= Addr.m_Addr.u.Word[1];
		Result.m_Addr.u.Word[2] ^= Addr.m_Addr.u.Word[2];
		Result.m_Addr.u.Word[3] ^= Addr.m_Addr.u.Word[3];
		Result.m_Addr.u.Word[4] ^= Addr.m_Addr.u.Word[4];
		Result.m_Addr.u.Word[5] ^= Addr.m_Addr.u.Word[5];
		Result.m_Addr.u.Word[6] ^= Addr.m_Addr.u.Word[6];
		Result.m_Addr.u.Word[7] ^= Addr.m_Addr.u.Word[7];

		return Result;
	}

	/*IPv6AddressImpl IPv6AddressImpl::operator &(const IPv6AddressImpl& Addr) const
	{
		IPv6AddressImpl Result(this);
		if (m_nScopte != Addr.m_nScopte)
			return Result;

		Result.m_Addr.u.Word[0] &= Addr.m_Addr.u.Word[0];
		Result.m_Addr.u.Word[1] &= Addr.m_Addr.u.Word[1];
		Result.m_Addr.u.Word[2] &= Addr.m_Addr.u.Word[2];
		Result.m_Addr.u.Word[3] &= Addr.m_Addr.u.Word[3];
		Result.m_Addr.u.Word[4] &= Addr.m_Addr.u.Word[4];
		Result.m_Addr.u.Word[5] &= Addr.m_Addr.u.Word[5];
		Result.m_Addr.u.Word[6] &= Addr.m_Addr.u.Word[6];
		Result.m_Addr.u.Word[7] &= Addr.m_Addr.u.Word[7];

		return Result;
	}*/

	IPv6AddressImpl IPv6AddressImpl::operator ~ () const
	{
		IPv6AddressImpl Result(this);
		Result.m_Addr.u.Word[0] ^= 0xFFFF;
		Result.m_Addr.u.Word[1] ^= 0xFFFF;
		Result.m_Addr.u.Word[2] ^= 0xFFFF;
		Result.m_Addr.u.Word[3] ^= 0xFFFF;
		Result.m_Addr.u.Word[4] ^= 0xFFFF;
		Result.m_Addr.u.Word[5] ^= 0xFFFF;
		Result.m_Addr.u.Word[6] ^= 0xFFFF;
		Result.m_Addr.u.Word[7] ^= 0xFFFF;

		return Result;
	}

	BOOL IPv6AddressImpl::operator == (const IPv6AddressImpl& Addr) const
	{
		return m_nScope == Addr.m_nScope && memcmp(&Addr.m_Addr, &Addr.m_Addr, sizeof(IN6_ADDR));
	}

	BOOL IPv6AddressImpl::operator !=(const IPv6AddressImpl& Addr) const
	{
		return !(*this == Addr);
	}

#endif
}
