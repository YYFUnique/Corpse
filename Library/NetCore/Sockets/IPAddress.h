#pragma once

namespace NetCore
{
	typedef AddressFamily::Family Family;
	static const Family IPv4 = AddressFamily::IPv4;
	static const Family IPv6 = AddressFamily::IPv6;
	
	class NET_API IPAddress
	{
	public:
		typedef std::vector<IPAddress> List;
		/// Creates a wildcard (zero) IPv4 IPAddress.
		IPAddress();		

		/// Creates an IPAddress by copying another one.
		IPAddress(const IPAddress& Addr);		

		/// Creates a wildcard (zero) IPAddress for the given address family.
		explicit IPAddress(Family family);
	
		/// Creates an IPAddress from the string containing
		/// an IP address in presentation format (dotted decimal
		/// for IPv4, hex string for IPv6).
		explicit IPAddress(LPCTSTR lpszAddr);
		
		/// Creates an IPAddress from the string containing
		/// an IP address in presentation format (dotted decimal
		/// for IPv4, hex string for IPv6).
		IPAddress(LPCTSTR& lpszAddr, Family family);
		
		/// Creates an IPAddress from a native internet address.
		/// A pointer to a in_addr or a in6_addr structure may be 
		/// passed.
		IPAddress(const void* pAddr, poco_socklen_t nLen);
		
		/// Creates an IPAddress from a native internet address.
		/// A pointer to a in_addr or a in6_addr structure may be 
		/// passed. Additionally, for an IPv6 address, a scope ID
		/// may be specified. The scope ID will be ignored if an IPv4
		/// address is specified.
		IPAddress(const void* pAddr, poco_socklen_t nLen, UINT nScope);

		/// Creates an IPAddress mask with the given length of prefix.
		IPAddress(unsigned prefix, Family family);
		
		/// Creates an IPAddress from Windows SOCKET_ADDRESS structure.
		IPAddress(const SOCKET_ADDRESS& socket_address);
		
		IPAddress(const SOCKADDR& SockAddr);

		~IPAddress();
	public:
		/// Assigns an IPAddress.
		IPAddress& operator = (const IPAddress& Addr);

		Family GetFamily() const;

		/// Returns the IPv6 scope identifier of the address. Returns 0 if
		/// the address is an IPv4 address, or the address is an
		/// IPv6 address but does not have a scope identifier.
		UINT GetScope() const;
		
		/// Returns a string containing a representation of the address
		/// in presentation format.
		LPCTSTR ToString() const;
		
		/// Returns true iff the address is a wildcard (all zero) address.
		BOOL IsWildcard() const;
		
		/// Returns true iff the address is a broadcast address.
		///
		/// Only IPv4 addresses can be broadcast addresses. In a broadcast
		/// address, all bits are one.
		///
		/// For an IPv6 address, returns always false.
		BOOL IsBroadcast() const;
		
		/// Returns true iff the address is a loopback address.
		/// For IPv4, the loopback address is 127.0.0.1.
		/// For IPv6, the loopback address is ::1.
		BOOL IsLoopback() const;

		/// Returns true iff the address is a multicast address.
		///
		/// IPv4 multicast addresses are in the  224.0.0.0 to 239.255.255.255 range
		/// (the first four bits have the value 1110).
		///
		/// IPv6 multicast addresses are in the FFxx:x:x:x:x:x:x:x range.
		BOOL IsMulticast() const;

		/// Returns true iff the address is a unicast address.
		///
		/// An address is unicast if it is neither a wildcard, broadcast or multicast address.
		BOOL IsUnicast() const;

		/// Returns true iff the address is a link local unicast address.
		///
		/// IPv4 link local addresses are in the 169.254.0.0/16 range,  according to RFC 3927.
		///
		/// IPv6 link local addresses have 1111 1110 10 as the first 10 bits, followed by 54 zeros.
		BOOL IsLinkLocal() const;

		/// Returns true iff the address is a site local unicast address.
		///
		/// IPv4 site local addresses are in on of the 10.0.0.0/24,
		/// 192.168.0.0/16 or 172.16.0.0 to 172.31.255.255 ranges.
		///
		/// Originally, IPv6 site-local addresses had FEC0/10 (1111 1110 11) 
		/// prefix (RFC 4291), followed by 38 zeros. Interfaces using  
		/// this mask are supported, but obsolete; RFC 4193 prescribes
		/// fc00::/7 (1111 110) as local unicast prefix.
		BOOL IsSiteLocal() const;
		
		/// Returns true iff the address is IPv4 compatible.
		///
		/// For IPv4 addresses, this is always true.
		///
		/// For IPv6, the address must be in the ::x:x range (the first 96 bits are zero).
		BOOL IsIPv4Compatible() const;

		/// Returns true iff the address is an IPv4 mapped IPv6 address.
		///
		/// For IPv4 addresses, this is always true.
		///
		/// For IPv6, the address must be in the ::FFFF:x:x range.
		BOOL IsIPv4Mapped() const;
		
		/// Returns true iff the address is a well-known multicast address.
		///
		/// For IPv4, well-known multicast addresses are in the 
		/// 224.0.0.0/8 range.
		///
		/// For IPv6, well-known multicast addresses are in the 
		/// FF0x:x:x:x:x:x:x:x range.
		BOOL IsWellKnownMC() const;
		
		/// Returns true iff the address is a node-local multicast address.
		///
		/// IPv4 does not support node-local addresses, thus the result is
		/// always false for an IPv4 address.
		///
		/// For IPv6, node-local multicast addresses are in the
		/// FFx1:x:x:x:x:x:x:x range.
		BOOL IsNodeLocalMC() const;

		/// Returns true iff the address is a link-local multicast address.
		///
		/// For IPv4, link-local multicast addresses are in the
		/// 224.0.0.0/24 range. Note that this overlaps with the range for well-known
		/// multicast addresses.
		///
		/// For IPv6, link-local multicast addresses are in the
		/// FFx2:x:x:x:x:x:x:x range.
		BOOL IsLinkLocalMC() const;
		
		/// Returns true iff the address is a site-local multicast address.
		///
		/// For IPv4, site local multicast addresses are in the
		/// 239.255.0.0/16 range.
		///
		/// For IPv6, site-local multicast addresses are in the
		/// FFx5:x:x:x:x:x:x:x range.
		BOOL IsSiteLocalMC() const;
		
		/// Returns true iff the address is a organization-local multicast address.
		///
		/// For IPv4, organization-local multicast addresses are in the
		/// 239.192.0.0/16 range.
		///
		/// For IPv6, organization-local multicast addresses are in the
		/// FFx8:x:x:x:x:x:x:x range.
		BOOL IsOrgLocalMC() const;
	
		/// Returns true iff the address is a global multicast address.
		///
		/// For IPv4, global multicast addresses are in the 
		/// 224.0.1.0 to 238.255.255.255 range.
		///
		/// For IPv6, global multicast addresses are in the
		/// FFxF:x:x:x:x:x:x:x range.
		BOOL IsGlobalMC() const;

		public:
			/// Returns the length in bytes of the internal socket address structure.	
			int GetLength() const;
			
			/// Returns the internal address structure.
			const void* GetAddr() const;
			
			/// Returns the address family (AF_INET or AF_INET6) of the address.
			int GetAF() const;
			
			/// Returns the prefix length.
			int GetPrefixLength() const;

			/// Masks the IP address using the given netmask, which is usually a IPv4 subnet mask. Only supported for IPv4 addresses.
			///
			/// The new address is (address & mask).
			void Mask(const IPAddress& IPMask);
			
			/// Masks the IP address using the given netmask, which is usually  a IPv4 subnet mask. Only supported for IPv4 addresses.
			///
			/// The new address is (address & mask) | (set & ~mask).
			void Mask(const IPAddress& IPMask, const IPAddress& IPSet);
			
			/// Creates an IPAddress from the string containing  an IP address in presentation format (dotted decimal for IPv4, hex string for IPv6).
			/// 
			/// Depending on the format of addr, either an IPv4 or an IPv6 address is created.
			///
			/// See toString() for details on the supported formats.
			///
			/// Throws an InvalidAddressException if the address cannot be parsed.
			static IPAddress Parse(LPCTSTR lpszAddr);
			
			/// Tries to interpret the given address string as an IP address in presentation format (dotted decimal for IPv4, hex string for IPv6).
			///
			/// Returns true and stores the IPAddress in result if the string contains a valid address.
			///
			/// Returns false and leaves result unchanged otherwise.
			static BOOL TryParse(LPCTSTR lpszAddr, IPAddress& IPAddrResult);

			/// Returns a wildcard IPv4 or IPv6 address (0.0.0.0).
			static IPAddress GetWildcard(Family family = IPv4);
			
			/// Returns a broadcast IPv4 address (255.255.255.255).
			static IPAddress GetBroadcast();
			
		public:
			BOOL operator == (const IPAddress& Addr) const;	
			BOOL operator != (const IPAddress& Addr) const;
			BOOL operator <  (const IPAddress& Addr) const;
			BOOL operator <= (const IPAddress& Addr) const;
			BOOL operator >  (const IPAddress& Addr) const;
			BOOL operator >= (const IPAddress& Addr) const;
			IPAddress operator & (const IPAddress& Addr) const;
			IPAddress operator | (const IPAddress& Addr) const;
			IPAddress operator ^ (const IPAddress& Addr) const;
			IPAddress operator ~ () const;

		private:
			IPAddressImpl* GetImpl() const;
			void NewIPv4();
			void NewIPv4(const void* pHostAddr);
			//void NewIPv4();
//#ifdef POCO_HAVE_IPv6
			void NewIPv6();
			void NewIPv6(const void* pHostAddr);
			void NewIPv6(const void* pHostAddr, UINT nScope);
//#endif
		private:
			IPAddressImpl* m_pImpl;
	};
}