#include "StdAfx.h"
#include "SocketAddressImpl.h"


namespace NetCore
{
	/////////////////////////////////////////////////////////////////////////////////
	SocketAddressImpl::SocketAddressImpl()
	{

	}

	SocketAddressImpl::~SocketAddressImpl()
	{

	}

	////////////////////////////////////////////////////////////////////////////////
	////
	////////////////////////////////////////////////////////////////////////////////
	IPv4SocketAddressImpl::IPv4SocketAddressImpl()
	{
		ZeroMemory(&m_Addr, sizeof(SOCKADDR_IN));
		m_Addr.sin_family = AF_INET;
	}

	IPv4SocketAddressImpl::IPv4SocketAddressImpl(const SOCKADDR_IN* pAddr)
	{
		memcpy(&m_Addr, pAddr, sizeof(SOCKADDR_IN));
	}

	IPv4SocketAddressImpl::IPv4SocketAddressImpl(const void* pAddr, WORD wPort)
	{
		ZeroMemory(&m_Addr, sizeof(SOCKADDR_IN));
		m_Addr.sin_family = AF_INET;
		memcpy(&m_Addr.sin_addr, pAddr, sizeof(m_Addr.sin_addr));
		m_Addr.sin_port = wPort;
	}

	LPCTSTR IPv4SocketAddressImpl::ToString() const
	{
		CString strResult;
		strResult.Format(_T("%s:%d"),GetHost().ToString(), ntohs(GetPort()));
		return strResult;
	}

	IPAddress IPv4SocketAddressImpl::GetHost() const
	{
		return IPAddress(&m_Addr.sin_addr, sizeof(m_Addr.sin_addr));
	}

	WORD IPv4SocketAddressImpl::GetPort() const
	{
		return m_Addr.sin_port;
	}

	poco_socklen_t IPv4SocketAddressImpl::GetLen() const
	{
		return sizeof(m_Addr);
	}

	const SOCKADDR* IPv4SocketAddressImpl::GetAddr() const
	{
		return reinterpret_cast<const SOCKADDR*>(&m_Addr);
	}

	int IPv4SocketAddressImpl::GetAF() const
	{
		return m_Addr.sin_family;
	}

	Family IPv4SocketAddressImpl::GetFamily() const
	{
		return AddressFamily::IPv4;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	////
	/////////////////////////////////////////////////////////////////////////////////////
	IPv6SocketAddressImpl::IPv6SocketAddressImpl(const SOCKADDR_IN6* pAddr)
	{
		memcpy(&m_Addr, pAddr, sizeof(SOCKADDR_IN6));
	}

	IPv6SocketAddressImpl::IPv6SocketAddressImpl(const void* pAddr, WORD wPort)
	{
		ZeroMemory(&m_Addr, sizeof(SOCKADDR_IN6));
		m_Addr.sin6_family = AF_INET6;
		memcpy(&m_Addr.sin6_addr, pAddr, sizeof(m_Addr.sin6_addr));
		m_Addr.sin6_port = wPort;
	}

	IPv6SocketAddressImpl::IPv6SocketAddressImpl(const void* pAddr, WORD wPort, UINT uScope)
	{
		ZeroMemory(&m_Addr, sizeof(SOCKADDR_IN6));
		m_Addr.sin6_family = AF_INET6;
		memcpy(&m_Addr.sin6_addr, pAddr, sizeof(m_Addr.sin6_addr));
		m_Addr.sin6_port = wPort;
		m_Addr.sin6_scope_id = uScope;
	}

	LPCTSTR IPv6SocketAddressImpl::ToString() const
	{
		CString strResult;
		strResult.Format(_T("[%s]:%d"),GetHost().ToString(), ntohs(GetPort()));
		return strResult;
	}

	IPAddress IPv6SocketAddressImpl::GetHost() const
	{
		return IPAddress(&m_Addr.sin6_addr, sizeof(m_Addr.sin6_addr), m_Addr.sin6_scope_id);
	}

	WORD IPv6SocketAddressImpl::GetPort() const
	{
		return m_Addr.sin6_port;
	}

	poco_socklen_t IPv6SocketAddressImpl::GetLen() const
	{
		return sizeof(m_Addr);
	}

	const SOCKADDR* IPv6SocketAddressImpl::GetAddr() const
	{
		return reinterpret_cast<const SOCKADDR*>(&m_Addr);
	}

	int IPv6SocketAddressImpl::GetAF() const
	{
		return m_Addr.sin6_family;
	}

	Family IPv6SocketAddressImpl::GetFamily()  const
	{
		return AddressFamily::IPv6;
	}
}