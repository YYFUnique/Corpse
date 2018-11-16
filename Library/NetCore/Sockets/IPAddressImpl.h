#pragma once

namespace NetCore
{
	class NET_API IPAddressImpl 
	{
	public:
		virtual ~IPAddressImpl();

		virtual IPAddressImpl* Clone() const = 0;
		virtual LPCTSTR ToString() const = 0;
		virtual poco_socklen_t GetLen() const = 0;
		virtual const void* GetAddr() const = 0;
		virtual Family GetFamily() const = 0;
		virtual int GetAF() const = 0;
		virtual UINT GetScope() const = 0;
		virtual BOOL IsWildcard() const = 0;
		virtual BOOL IsBroadcast() const = 0;
		virtual BOOL IsLoopback() const = 0;
		virtual BOOL IsMulticast() const = 0;
		virtual BOOL IsLinkLocal() const = 0;
		virtual BOOL IsSiteLocal() const = 0;
		virtual BOOL IsIPv4Mapped() const = 0;
		virtual BOOL IsIPv4Compatible() const = 0;
		virtual BOOL IsWellKnownMC() const = 0;
		virtual BOOL IsNodeLocalMC() const = 0;
		virtual BOOL IsLinkLocalMC() const = 0;
		virtual BOOL IsSiteLocalMC() const = 0;
		virtual BOOL IsOrgLocalMC() const = 0;
		virtual BOOL IsGlobalMC() const = 0;
		virtual void Mask(const IPAddressImpl* pMask, const IPAddressImpl* pSet) = 0;
		virtual int GetPrefixLength() const = 0;
	protected:
		IPAddressImpl();
	private:
		IPAddressImpl(const IPAddressImpl&);
		IPAddressImpl& operator = (const IPAddressImpl& );
	};

	//
	// IPv4AddressImpl
	//

	class NET_API IPv4AddressImpl : public IPAddressImpl
	{
	public:
		IPv4AddressImpl();
		IPv4AddressImpl(unsigned prefix);
		IPv4AddressImpl(const void* pAddr);
		//IPv4AddressImpl()
		IPv4AddressImpl(const IPv4AddressImpl& Addr);
		IPv4AddressImpl& operator = (const IPv4AddressImpl&);
	public:
		virtual LPCTSTR ToString() const;
		virtual poco_socklen_t GetLen() const;
		virtual const void* GetAddr() const;
		virtual Family GetFamily() const;
		virtual int GetAF() const;
		virtual UINT GetScope() const;
		virtual BOOL IsWildcard() const;
		virtual BOOL IsBroadcast() const;
		virtual BOOL IsLoopback() const;
		virtual BOOL IsMulticast() const;
		virtual BOOL IsLinkLocal() const;
		virtual BOOL IsSiteLocal() const;
		virtual BOOL IsIPv4Mapped() const;
		virtual BOOL IsIPv4Compatible() const;
		virtual BOOL IsWellKnownMC() const;
		virtual BOOL IsNodeLocalMC() const;
		virtual BOOL IsLinkLocalMC() const;
		virtual BOOL IsSiteLocalMC() const;
		virtual BOOL IsOrgLocalMC() const;
		virtual BOOL IsGlobalMC() const;
		virtual int GetPrefixLength() const;
		virtual void Mask(const IPAddressImpl* pMask, const IPAddressImpl* pSet);
		virtual IPAddressImpl* Clone() const;
	public:
		IPv4AddressImpl operator & (const IPv4AddressImpl& Addr) const;
		IPv4AddressImpl operator | (const IPv4AddressImpl& Addr) const;
		IPv4AddressImpl operator ^ (const IPv4AddressImpl& Addr) const;
		IPv4AddressImpl operator ~ () const;
		BOOL operator == (const IPv4AddressImpl& Addr) const;
		BOOL operator != (const IPv4AddressImpl& Addr) const;
	public:
		static IPv4AddressImpl Parse(LPCTSTR lpszAddr);
	private:
		IN_ADDR m_Addr;
	};

#ifdef POCO_HAVE_IPv6

	class NET_API IPv6AddressImpl : public IPAddressImpl
	{
	public:
		IPv6AddressImpl();
		IPv6AddressImpl(unsigned prefix);
		IPv6AddressImpl(const void* pAddr);
		IPv6AddressImpl(const IPv6AddressImpl* pAddr);
		IPv6AddressImpl(const IPv6AddressImpl& Addr);
		IPv6AddressImpl(const void* pAddr, UINT nScope);

	public:
		virtual LPCTSTR ToString() const;
		virtual poco_socklen_t GetLen() const;
		virtual const void* GetAddr() const;
		virtual Family GetFamily() const;
		virtual int GetAF() const;
		virtual UINT GetScope() const;
		virtual BOOL IsWildcard() const;
		virtual BOOL IsBroadcast() const;
		virtual BOOL IsLoopback() const;
		virtual BOOL IsMulticast() const;
		virtual BOOL IsLinkLocal() const;
		virtual BOOL IsSiteLocal() const;
		virtual BOOL IsIPv4Compatible() const;
		virtual BOOL IsIPv4Mapped() const;
		virtual BOOL IsWellKnownMC() const;
		virtual BOOL IsNodeLocalMC() const;
		virtual BOOL IsLinkLocalMC() const;
		virtual BOOL IsSiteLocalMC() const;
		virtual BOOL IsOrgLocalMC() const;
		virtual BOOL IsGlobalMC() const;
		virtual int GetPrefixLength() const;
		virtual void Mask(const IPAddressImpl* pMask, const IPAddressImpl* pSet);
		IPAddressImpl* Clone() const;
	public:
		IPv6AddressImpl operator & (const IPv6AddressImpl& Addr) const;
		IPv6AddressImpl operator | (const IPv6AddressImpl& Addr) const;
		IPv6AddressImpl operator ^ (const IPv6AddressImpl& Addr) const;
		IPv6AddressImpl operator ~ () const;
		BOOL operator == (const IPv6AddressImpl& Addr) const;
		BOOL operator != (const IPv6AddressImpl& Addr) const;
	public:
		static IPv6AddressImpl Parse(LPCTSTR lpszAddr);
	private:
		IN6_ADDR	m_Addr;
		UINT			m_nScope;
	};
#endif
}
