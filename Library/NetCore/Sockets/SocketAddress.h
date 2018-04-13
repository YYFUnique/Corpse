#pragma once

namespace NetCore
{
	class NET_API SocketAddress
	{
	public:
		static const Family IPv4 = AddressFamily::IPv4;
		static const Family IPv6 = AddressFamily::IPv6;

	public:
		/// Creates a wildcard (all zero) IPv4 SocketAddress.
		SocketAddress();
		/// Creates a SocketAddress with unspecified (wildcard) IP address of the given family.
		//	explicit关键字只能用于修饰只有一个参数的类构造函数, 它的作用是表明该构造函数是显示的
		//	跟它相对应的另一个关键字是implicit, 意思是隐藏的
		explicit SocketAddress(Family family);
		
		/// Creates a SocketAddress from an IP address and given port number.
		SocketAddress(const IPAddress& HostAddress, WORD wPortNumber);

		/// Creates a SocketAddress from an IP address and given port number.
		SocketAddress(LPCTSTR lpszHostAddress, WORD wPortNumber);

		SocketAddress(WORD wPortNumber);

		SocketAddress(Family family, WORD wPortNumber);

		/// Creates a SocketAddress from an IP address and given port number.
		SocketAddress(Family family, LPCTSTR lpszHostAddress, WORD wPortNumber);

		/// Creates a SocketAddress from an IP address and the service name or port number.
		SocketAddress(LPCTSTR lpszHostAddress, LPCTSTR lpszPortNumber);

		/// Creates a SocketAddress from an IP address and the service name or port number.
		SocketAddress(Family family, LPCTSTR lpszHostAddress, LPCTSTR lpszPortNumber);

		/// Examples:
		///     192.168.1.10:80
		///     [::ffff:192.168.1.120]:2040
		///     www.appinf.com:8080
		explicit SocketAddress(LPCTSTR lpszHostAndPort);

		SocketAddress(const SocketAddress& Addr);
		/// Creates a SocketAddress by copying another one.

		SocketAddress(const SOCKADDR* pAddr, poco_socklen_t nLe);
		/// Creates a SocketAddress from a native socket address.

		~SocketAddress();

	public:
		SocketAddress& operator = (const SocketAddress& SocketAddr);
		BOOL operator < (const SocketAddress& SocketAddr) const;
		BOOL operator == (const SocketAddress& SocketAddr) const;
		BOOL operator != (const SocketAddress& SocketAddr) const;

	public:
		/// Returns the host IP address.
		IPAddress GetHost() const;

		/// Returns the port number.
		WORD GetPort() const;
		
		poco_socklen_t GetLen() const;

		const SOCKADDR* GetAddr() const;

		/// Returns the address family (AF_INET or AF_INET6) of the address.
		int GetAF() const;
		
		/// Returns a string representation of the address.
		LPCTSTR ToString() const;
		
		/// Returns the address family of the host's address.
		Family GetFamily() const;

	protected:
		void Init(const IPAddress& HostAddress, WORD wPortNumber);
		void Init(LPCTSTR lpszHostAddress, WORD wPortNumber);
		void Init(Family family, LPCTSTR lpszHostAddress, WORD wPortNumber);
		void Init(Family family, LPCTSTR lpszAddress);
		void Init(LPCTSTR lpszHostAndPort);
		WORD ResolveService(LPCTSTR lpszService);

	private:
		void NewIPv4();
		void NewIPv4(const SOCKADDR_IN* pSockAddr);
		void NewIPv4(const IPAddress& HostAddr, WORD wPortNumber);

		void NewIPv6(const SOCKADDR_IN6* pSockAddr);
		void NewIPv6(const IPAddress& HostAddr, WORD wPortNumber);

	private:
		SocketAddressImpl* m_pImpl;
	};

	//NET_API BinaryWriter& operator << (BinaryWriter& writer, const SocketAddress& value);
	//NET_API BinaryReader& operator >> (BinaryReader& reader, const SocketAddress& value);
}