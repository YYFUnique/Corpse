#pragma once

namespace NetCore
{
	class NET_API SocketAddressImpl : public RefCountObject
	{
	public:
		virtual ~SocketAddressImpl();
		virtual IPAddress GetHost() const = 0;
		virtual WORD	GetPort() const = 0;
		virtual poco_socklen_t GetLen() const = 0;
		virtual int GetAF() const = 0;
		virtual const SOCKADDR* GetAddr() const = 0;
		virtual Family GetFamily() const = 0;
		virtual LPCTSTR ToString() const = 0;
	protected:
		SocketAddressImpl();
	private:
		SocketAddressImpl(const SocketAddressImpl&);
		SocketAddressImpl& operator = (const SocketAddressImpl&);
	};

	class NET_API IPv4SocketAddressImpl : public SocketAddressImpl
	{
	public:
		IPv4SocketAddressImpl();
		IPv4SocketAddressImpl(const SOCKADDR_IN* pAddr);
		IPv4SocketAddressImpl(const void* pAddr, WORD wPort);

	public:
		virtual IPAddress GetHost() const;
		virtual WORD		GetPort() const;
		virtual poco_socklen_t GetLen() const;
		virtual int GetAF() const;
		virtual const SOCKADDR* GetAddr() const;
		virtual Family GetFamily() const;
		virtual LPCTSTR ToString() const;		
	private:
		SOCKADDR_IN m_Addr;
	};

	class NET_API IPv6SocketAddressImpl : public SocketAddressImpl
	{
	public:
		IPv6SocketAddressImpl(const SOCKADDR_IN6* pAddr);
		IPv6SocketAddressImpl(const void* pAddr, WORD wPort);
		IPv6SocketAddressImpl(const void* pAddr, WORD wPort, UINT uScope);
	public:
		virtual IPAddress GetHost() const;
		virtual WORD	GetPort() const;
		virtual poco_socklen_t GetLen() const;
		virtual int GetAF() const;
		virtual Family GetFamily() const;
		virtual LPCTSTR ToString() const;
		virtual const SOCKADDR* GetAddr() const;
	private:
		SOCKADDR_IN6 m_Addr;
	};
}