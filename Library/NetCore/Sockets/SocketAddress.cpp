#include "Stdafx.h"
#include "SocketAddress.h"

namespace NetCore
{
	struct AFLT
	{
		bool operator () (const IPAddress& a1, const IPAddress& a2)
		{
			return a1.GetAF() < a2.GetAF();
		}
	};

	SocketAddress::SocketAddress()
	{
		NewIPv4();
	}

	SocketAddress::SocketAddress(Family Family)
	{
		Init(IPAddress(Family), 0);
	}

	SocketAddress::SocketAddress(WORD wPortNumber)
	{
		Init(IPAddress(), wPortNumber);
	}

	SocketAddress::SocketAddress(Family family, WORD wPortNumber)
	{
		Init(IPAddress(family), wPortNumber);
	}

	SocketAddress::SocketAddress(LPCTSTR lpszHostAddress, LPCTSTR lpszPortNumber)
	{
		Init(lpszHostAddress, ResolveService(lpszPortNumber));
	}

	SocketAddress::SocketAddress(Family family, LPCTSTR lpszHostAddress, LPCTSTR lpszPortNumber)
	{
		Init(family, lpszHostAddress, ResolveService(lpszPortNumber));
	}

	SocketAddress::SocketAddress(const IPAddress& HostAddress, WORD wPortNumber)
	{
		Init(HostAddress, wPortNumber);
	}

	SocketAddress::SocketAddress(LPCTSTR lpszHostAddress, WORD wPortNumber)
	{
		Init(lpszHostAddress, wPortNumber);
	}

	SocketAddress::SocketAddress(const SocketAddress& Addr)
	{
		if (Addr.GetFamily() == IPv4)
			NewIPv4(reinterpret_cast<const SOCKADDR_IN*>(Addr.GetAddr()));
		else if (Addr.GetFamily() == IPv6)
			NewIPv6(reinterpret_cast<const SOCKADDR_IN6*>(Addr.GetAddr()));
	}

	SocketAddress::SocketAddress(const SOCKADDR* pAddr, poco_socklen_t nLen)
	{
		if (nLen == sizeof(SOCKADDR_IN) && pAddr->sa_family == AF_INET )
			NewIPv4(reinterpret_cast<const SOCKADDR_IN*>(pAddr));
		else if (nLen == sizeof(SOCKADDR_IN6) && pAddr->sa_family == AF_INET6)
			NewIPv6(reinterpret_cast<const SOCKADDR_IN6*>(pAddr));
	}

	SocketAddress::~SocketAddress()
	{

	}

	BOOL SocketAddress::operator < (const SocketAddress& SocketAddr) const
	{
		if (GetFamily() < SocketAddr.GetFamily())
			return TRUE;
		if (GetFamily() > SocketAddr.GetFamily())
			return FALSE;

		if (GetHost() < SocketAddr.GetHost())
			return TRUE;
		if (GetHost() > SocketAddr.GetHost())
			return FALSE;

		return (GetPort() < SocketAddr.GetPort());
	}

	SocketAddress& SocketAddress::operator= (const SocketAddress& SocketAddr)
	{
		if (&SocketAddr != this)
		{
			if (SocketAddr.GetFamily() == IPv4)
				NewIPv4(reinterpret_cast<const SOCKADDR_IN*>(SocketAddr.GetAddr()));
			else if (SocketAddr.GetFamily() == IPv6)
				NewIPv6(reinterpret_cast<const SOCKADDR_IN6*>(SocketAddr.GetAddr()));
		}

		return *this;
	}

	BOOL SocketAddress::operator == (const SocketAddress& SockAddr) const
	{
		return GetHost() == SockAddr.GetHost() && GetPort() == SockAddr.GetPort();
	}

	BOOL SocketAddress::operator !=(const SocketAddress& SockAddr) const
	{
		return !(operator == (SockAddr));
	}

	IPAddress SocketAddress::GetHost() const
	{
		return m_pImpl->GetHost();
	}

	WORD SocketAddress::GetPort() const
	{
		return ntohs(m_pImpl->GetPort());
	}

	poco_socklen_t SocketAddress::GetLen() const
	{
		return m_pImpl->GetLen();
	}

	const SOCKADDR* SocketAddress::GetAddr() const
	{
		return m_pImpl->GetAddr();
	}

	int SocketAddress::GetAF() const
	{
		return m_pImpl->GetAF();
	}

	Family SocketAddress::GetFamily() const
	{
		return static_cast<Family>(m_pImpl->GetFamily());
	}

	LPCTSTR SocketAddress::ToString() const
	{
		return m_pImpl->ToString();
	}

	void SocketAddress::Init(const IPAddress& HostAddress, WORD wPortNumber)
	{
		if (HostAddress.GetFamily() == IPv4)
			NewIPv4(HostAddress, wPortNumber);
		else if (HostAddress.GetFamily() == IPv6)
			NewIPv6(HostAddress, wPortNumber);
	}

	void SocketAddress::Init(LPCTSTR lpszHostAddress, WORD wPortNumber)
	{
		IPAddress IpAddr;
		if (IPAddress::TryParse(lpszHostAddress, IpAddr))
			Init(IpAddr, wPortNumber);
		else
		{
			HostEntry he = DNS::GetHostByName(lpszHostAddress);
			AddressList Address = he.GetAddress();
			if (Address.size() > 0)
			{
#if defined(POCO_HAVE_IPv6)
				// if we get both IPv4 and IPv6 addresses, prefer IPv4
				std::stable_sort(Address.begin(), Address.end(), AFLT());
#endif
				Init(Address[0], wPortNumber);
			}
		}
	}

	void SocketAddress::Init(Family family, LPCTSTR lpszHostAddress, WORD wPortNumber)
	{
		IPAddress ip;
		if (IPAddress::TryParse(lpszHostAddress, ip))
		{
			if (ip.GetFamily() != family)
				return;
			Init(ip, wPortNumber);
		}
		else
		{
			HostEntry he = DNS::GetHostByName(lpszHostAddress);
			AddressList Address = he.GetAddress();
			if (Address.size() > 0)
			{
				for (AddressList::const_iterator it = Address.begin(); it != Address.end(); ++it)
				{
					if (it->GetFamily() == family)
					{
						Init(*it, wPortNumber);
						return;
					}
				}
			}
		}
	}

	void SocketAddress::Init(Family family, LPCTSTR lpszAddress)
	{
		CString strHost;
		CString	 strPort;
		
		/*if (lpszAddress[0] == _T("["))
		{
			++lpszAddress;
			while
		}*/


		Init(family, strHost, ResolveService(strPort));
	}

	WORD SocketAddress::ResolveService(LPCTSTR lpszService)
	{
		return 0;
	}

	/*SocketAddressImpl* SocketAddress::get*/

	void SocketAddress::NewIPv4()
	{
		m_pImpl = new IPv4SocketAddressImpl;
	}

	void SocketAddress::NewIPv4(const SOCKADDR_IN* pSockAddr)
	{
		m_pImpl = new IPv4SocketAddressImpl(pSockAddr);
	}

	void SocketAddress::NewIPv4(const IPAddress& HostAddr, WORD wPortNumber)
	{
		m_pImpl = new IPv4SocketAddressImpl(HostAddr.GetAddr(), htons(wPortNumber));
	}

	void SocketAddress::NewIPv6(const SOCKADDR_IN6* pSockAddr)
	{
		m_pImpl = new IPv6SocketAddressImpl(pSockAddr);
	}

	void SocketAddress::NewIPv6(const IPAddress& HostAddr, WORD wPortNumber)
	{
		m_pImpl = new IPv6SocketAddressImpl(HostAddr.GetAddr(), htons((wPortNumber)));
	}
}