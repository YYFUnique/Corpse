#include "StdAfx.h"
#include "IPAddress.h"

namespace NetCore
{
	IPAddress::IPAddress()
	{
		NewIPv4();
	}

	IPAddress::IPAddress(const IPAddress& Addr)
	{
		if (Addr.GetFamily() == IPv4)
			NewIPv4(Addr.GetAddr());
		else if (Addr.GetFamily() == IPv6)
			NewIPv6(Addr.GetAddr(), Addr.GetScope());
	}

	IPAddress::IPAddress(Family family)
	{
		if (family == IPv4)
			NewIPv4();
		else if (family == IPv6)
			NewIPv6();
	}

	IPAddress::IPAddress(LPCTSTR lpszAddr)
	{
#ifdef POCO_HAVE_IPv6
		IPv6AddressImpl Empty6 = IPv6AddressImpl();
		if (lpszAddr == NULL || lpszAddr[0] == _T('\0') || _tcsicmp(lpszAddr, _T("::") == 0))
		{
			NewIPv6(Empty6.GetAddr());
			return;
		}

		IPv6AddressImpl Addr6(IPv6AddressImpl::Parse(lpszAddr));
		if (Addr6 != IPv6AddressImpl())
		{
			NewIPv6(Addr6.GetAddr(), Addr6.GetScope());
			return;
		}
#else
		IPv4AddressImpl Empty4 = IPv4AddressImpl();
		if (lpszAddr == NULL || lpszAddr[0] == _T('\0') || _tcsicmp(lpszAddr, _T("0.0.0.0")) == 0)
		{
			NewIPv4();
			return;
		}

		IPv4AddressImpl Addr4(IPv4AddressImpl::Parse(lpszAddr));
		if (Addr4 != Empty4)
		{
			NewIPv4(Addr4.GetAddr());
			return;
		}
#endif
	}

	IPAddress::IPAddress(LPCTSTR& lpszAddr, Family family)
	{
		if (family == IPv4)
		{
			IPv4AddressImpl Addr4(IPv4AddressImpl::Parse(lpszAddr));
			NewIPv4(Addr4.GetAddr());
		}
		else if (family == IPv6)
		{
#ifdef POCO_HAVE_IPv6
			IPv6AddressImpl Addr6(IPv6AddressImpl::Parse(lpszAddr));
			NewIPv6(Addr6.GetAddr());
#endif
		}
	}

	IPAddress::IPAddress(const void* pAddr, poco_socklen_t nLen)
	{
		if (nLen == sizeof(IN_ADDR))
			NewIPv4(pAddr);
		else if (nLen == sizeof(IN6_ADDR))
			NewIPv6(pAddr);
	}

	IPAddress::IPAddress(const void* pAddr, poco_socklen_t nLen, UINT nScope)
	{
		if (nLen == sizeof(IN_ADDR))
			NewIPv4(pAddr);
		else if (nLen == sizeof(IN6_ADDR))
			NewIPv6(pAddr);
	}

	IPAddress::IPAddress(const SOCKET_ADDRESS& SockAddr)
	{
		ADDRESS_FAMILY family = SockAddr.lpSockaddr->sa_family;
		if (family == AF_INET)
			NewIPv4(&reinterpret_cast<const SOCKADDR_IN*>(SockAddr.lpSockaddr)->sin_addr);
		else if (family == AF_INET6)
			NewIPv6(&reinterpret_cast<const SOCKADDR_IN6*>(SockAddr.lpSockaddr)->sin6_addr,
							reinterpret_cast<const SOCKADDR_IN6*>(SockAddr.lpSockaddr)->sin6_scope_id);
	}

	IPAddress::IPAddress(const SOCKADDR& SockAddr)
	{
		WORD wFamily = SockAddr.sa_family;
		if (wFamily == AF_INET)
			NewIPv4(&reinterpret_cast<const SOCKADDR_IN*>(&SockAddr)->sin_addr);
		else if (wFamily == AF_INET6)
			NewIPv6(&reinterpret_cast<const SOCKADDR_IN6*>(&SockAddr)->sin6_addr,
							reinterpret_cast<const SOCKADDR_IN6*>(&SockAddr)->sin6_scope_id);
	}

	IPAddress::~IPAddress()
	{

	}

	IPAddress& IPAddress::operator =(const IPAddress& IpAddr)
	{
		if (&IpAddr != this)
		{
			if (IpAddr.GetFamily() == IPv4)
				NewIPv4(IpAddr.GetAddr());
			else if (IpAddr.GetFamily() == IPv6)
				NewIPv6(IpAddr.GetAddr(), IpAddr.GetScope());
		}
		return *this;
	}

	Family IPAddress::GetFamily() const
	{
		return m_pImpl->GetFamily();
	}

	UINT IPAddress::GetScope() const
	{
		return m_pImpl->GetScope();
	}

	LPCTSTR IPAddress::ToString() const
	{
		return m_pImpl->ToString();
	}

	BOOL IPAddress::IsWildcard() const
	{
		return m_pImpl->IsWildcard();
	}

	BOOL IPAddress::IsBroadcast() const
	{
		return m_pImpl->IsBroadcast();
	}

	BOOL IPAddress::IsLoopback() const
	{
		return m_pImpl->IsLoopback();
	}

	BOOL IPAddress::IsMulticast() const
	{
		return m_pImpl->IsMulticast();
	}

	BOOL IPAddress::IsUnicast() const
	{
		return !IsWildcard() && !IsBroadcast() && !IsMulticast();
	}

	BOOL IPAddress::IsLinkLocal() const
	{
		return m_pImpl->IsLinkLocal();
	}

	BOOL IPAddress::IsSiteLocal() const
	{
		return m_pImpl->IsSiteLocal();
	}

	BOOL IPAddress::IsIPv4Compatible() const
	{
		return m_pImpl->IsIPv4Compatible();
	}

	BOOL IPAddress::IsIPv4Mapped() const
	{
		return m_pImpl->IsIPv4Mapped();
	}

	BOOL IPAddress::IsWellKnownMC() const
	{
		return m_pImpl->IsWellKnownMC();
	}

	BOOL IPAddress::IsNodeLocalMC() const
	{
		return m_pImpl->IsNodeLocalMC();
	}

	BOOL IPAddress::IsLinkLocalMC() const
	{
		return m_pImpl->IsLinkLocalMC();
	}

	BOOL IPAddress::IsSiteLocalMC() const
	{
		return m_pImpl->IsSiteLocalMC();
	}

	BOOL IPAddress::IsOrgLocalMC() const
	{
		return m_pImpl->IsOrgLocalMC();
	}

	BOOL IPAddress::IsGlobalMC() const
	{
		return m_pImpl->IsGlobalMC();
	}

	BOOL IPAddress::operator ==(const IPAddress& IpAddr) const
	{
		poco_socklen_t nLen1 = GetLength();
		poco_socklen_t nLen2 = IpAddr.GetLength();

		if (nLen1 == nLen2)
		{
#ifdef POCO_HAVE_IPv6
			if (GetScope() != IpAddr.GetScope())
				return FALSE;
#endif
			return memcmp(GetAddr(), IpAddr.GetAddr(), nLen1) == 0;
		}
		else
			return FALSE;
	}

	BOOL IPAddress::operator != (const IPAddress& IpAddr) const
	{
		return !(*this == IpAddr);
	}

	BOOL IPAddress::operator < (const IPAddress& IpAddr) const
	{
		poco_socklen_t nLen1 = GetLength();
		poco_socklen_t nLen2 = IpAddr.GetLength();

		if (nLen1 == nLen2)
		{
#ifdef POCO_HAVE_IPv6
			if (GetScope() != IpAddr.GetScope())
				return GetScope() < IpAddr.GetScope();
#endif
			return memcmp(GetAddr(), IpAddr.GetAddr(), nLen1) < 0;
		}
		else
			return nLen1 < nLen2;
	}

	BOOL IPAddress::operator <= (const IPAddress& IpAddr) const
	{
		return !(IpAddr < *this);
	}

	BOOL IPAddress::operator >(const IPAddress& IpAddr) const
	{
		return IpAddr < *this;
	}

	BOOL IPAddress::operator >=(const IPAddress& IpAddr) const
	{
		return !(*this < IpAddr);
	}

	IPAddress IPAddress::operator &(const IPAddress& IpAddr) const
	{
		if (GetFamily() == IpAddr.GetFamily())
			return IPAddress();

		if (GetFamily() == IPv4)
		{
			IPv4AddressImpl t(GetAddr());
			IPv4AddressImpl o(IpAddr.GetAddr());

			return IPAddress((t & o).GetAddr(), sizeof(IN_ADDR));
		}
		else if (GetFamily() == IPv6)
		{
#ifdef POCO_HAVE_IPv6
			const IPv6AddressImpl t(GetAddr(),GetScope());
			const IPv6AddressImpl o(IpAddr.GetAddr(), IpAddr.GetScope());
			const IPv6AddressImpl r = t & o;
			return IPAddress(r.GetAddr(), sizeof(IN6_ADDR), r.GetScope());
#endif
		}
		return IPAddress();
	}

	IPAddress IPAddress::operator |(const IPAddress& IpAddr) const
	{
		if (GetFamily() == IpAddr.GetFamily())
			return IPAddress();

		if (GetFamily() == IPv4)
		{
			IPv4AddressImpl t(GetAddr());
			IPv4AddressImpl o(IpAddr.GetAddr());
			return IPAddress((t | o).GetAddr(), sizeof(IN_ADDR));
		}
		else if (GetFamily() == IPv6)
		{
#ifdef POCO_HAVE_IPv6
			const IPv6AddressImpl t(GetAddr(), GetScope());
			const IPv6AddressImpl o(IpAddr.GetAddr(), IpAddr.GetScope());
			const IPv6AddressImpl r = t | o;
			return IPAddress(r.GetAddr(), sizeof(IN6_ADDR), r.GetScope());
#endif
		}

		return IPAddress();
	}

	IPAddress IPAddress::operator ^(const IPAddress& IpAddr) const
	{
		if (GetFamily() == IpAddr.GetFamily())
			return IPAddress();

		if (GetFamily() == IPv4)
		{
			IPv4AddressImpl t(GetAddr());
			IPv4AddressImpl o(IpAddr.GetAddr());
			return IPAddress((t ^ o).GetAddr(), sizeof(IN_ADDR));
		}
		else if (GetFamily() == IPv6)
		{
#ifdef POCO_HAVE_IPv6
			const IPv6AddressImpl t(GetAddr(), GetScope());
			const IPv6AddressImpl o(IpAddr.GetAddr(), IpAddr.GetScope());
			const IPv6AddressImpl r = t ^ o;
			return IPAddress(r.GetAddr(), sizeof(IN6_ADDR), r.GetScope());
#endif;
		}

		return IPAddress();
	}

	IPAddress IPAddress::operator ~() const
	{
		if (GetFamily() == IPv4)
		{
			IPv4AddressImpl self(GetAddr());
			return IPAddress((~self).GetAddr(), sizeof(IN_ADDR));
		}
		else if (GetFamily() == IPv6)
		{
#ifdef POCO_HAVE_IPv6
			IPv6AddressImpl self(GetAddr(), GetScope());
			const IPv6AddressImpl r = ~self;
			return IPAddress(r.GetAddr(), sizeof(IN6_ADDR), r.GetScope());
#endif
		}

		return IPAddress();
	}

	poco_socklen_t IPAddress::GetLength() const
	{
		return m_pImpl->GetLen();
	}

	const void* IPAddress::GetAddr() const
	{
		return m_pImpl->GetAddr();
	}

	int IPAddress::GetAF() const
	{
		return m_pImpl->GetAF();
	}

	IPAddress IPAddress::Parse(LPCTSTR lpszAddr)
	{
		return IPAddress(lpszAddr);
	}

	BOOL IPAddress::TryParse(LPCTSTR lpszAddr, IPAddress& IPAddrResult)
	{
#ifdef POCO_HAVE_IPv6
		IPv6AddressImpl impl6(IPv6AddressImpl::Parse(lpszAddr));
		if (impl6 != IPv6AddressImpl())
		{
			IPAddrResult.NewIPv6(impl6.GetAddr(), impl6.GetScope());
			return TRUE;
		}
#else
		IPv4AddressImpl impl4(IPv4AddressImpl::Parse(lpszAddr));
		if (impl4 != IPv4AddressImpl() || _tcsicmp(lpszAddr, _T("0.0.0.0")) == 0)
		{
			IPAddrResult.NewIPv4(impl4.GetAddr());
			return TRUE;
		}
#endif
		return FALSE;
	}

	void IPAddress::Mask(const IPAddress& IPMask)
	{
		IPAddress IPNull;
		m_pImpl->Mask(IPMask.GetImpl(), IPNull.GetImpl());
	}

	void IPAddress::Mask(const IPAddress& IPMask, const IPAddress& IPSet)
	{
		m_pImpl->Mask(IPMask.GetImpl(), IPSet.GetImpl());
	}

	IPAddress IPAddress::GetWildcard(Family family /* = IPv4 */)
	{
		return IPAddress(family);
	}

	IPAddress IPAddress::GetBroadcast()
	{
		IN_ADDR InAddr;
		InAddr.s_addr = INADDR_NONE;
		return IPAddress(&InAddr, sizeof(IN_ADDR));
	}

	IPAddressImpl* IPAddress::GetImpl() const
	{
		if (m_pImpl)
			return m_pImpl;
		return NULL;
	}

	void IPAddress::NewIPv4()
	{
		m_pImpl = new IPv4AddressImpl();
	}

	void IPAddress::NewIPv4(const void* pHostAddr)
	{
		m_pImpl = new IPv4AddressImpl(pHostAddr);
	}

	void IPAddress::NewIPv6()
	{
#ifdef POCO_HAVE_IPv6
		m_pImpl = new IPv6AddressImpl();
#endif
	}

	void IPAddress::NewIPv6(const void* pHostAddr)
	{
#ifdef POCO_HAVE_IPv6
		m_pImpl = new IPv6AddressImpl(pHostAddr);
#endif
	}
	
	void IPAddress::NewIPv6(const void* pHostAddr, UINT nScope)
	{
#ifdef POCO_HAVE_IPv6
		m_pImpl = new IPv6AddressImpl(pHostAddr, nScope);
#endif
	}
}
