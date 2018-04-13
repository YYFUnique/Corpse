#include "StdAfx.h"
#include "RawSocket.h"

namespace NetCore
{
	RawSocket::RawSocket()
		:Socket(new RawSocketImpl)
	{

	}

	RawSocket::RawSocket(Family family, int nProto /* = IPPROTO_RAW */)
		:Socket(new RawSocketImpl(family, nProto))
	{

	}

	RawSocket::RawSocket(const SocketAddress& Addr, BOOL bReuseAddr /* = FALSE */)
		: Socket(new RawSocketImpl(Addr.GetFamily()))
	{
		Bind(Addr, bReuseAddr);
	}

	RawSocket::RawSocket(const Socket& socket)
		:Socket(socket)
	{

	}

	RawSocket::RawSocket(SocketImpl* pImpl)
		: Socket(pImpl)
	{

	}

	RawSocket::~RawSocket()
	{

	}

	RawSocket& RawSocket::operator =(const Socket& socket)
	{
		Socket::operator =(socket);
		return *this;
	}

	void RawSocket::Connect(const SocketAddress& Addr)
	{
		GetImpl()->Connect(Addr);
	}

	void RawSocket::Bind(const SocketAddress& Addr, BOOL bReuseAddr, BOOL bReusePort)
	{
		GetImpl()->Bind(Addr, bReuseAddr, bReusePort);
	}

	void RawSocket::Bind(const SocketAddress& Addr, BOOL bReuseAddr /* = FALSE */)
	{
		GetImpl()->Bind(Addr, bReuseAddr);
	}

	int RawSocket::SendBytes(const void* pBuffer, int nLen, int nFlag /* = 0 */)
	{
		return GetImpl()->SendBytes(pBuffer, nLen, nFlag);
	}

	int RawSocket::ReceiveBytes(void* pBuffer, int nLen, int nFalg /* = 0 */)
	{
		return GetImpl()->ReceiveBytes(pBuffer, nLen, nFalg);
	}

	int RawSocket::SendTo(const void* pBuffer, int nLen, const SocketAddress& Addr, int nFlag /* =0 */)
	{
		return GetImpl()->SendTo(pBuffer, nLen, Addr, nFlag);
	}

	int RawSocket::ReceiveFrom(void* pBuffer, int nLen, SocketAddress& Addr, int nFlag /* = 0 */)
	{
		return GetImpl()->ReceiveFrom(pBuffer, nLen, Addr, nFlag);
	}

	void RawSocket::SetBroadcast(BOOL bFlag)
	{
		GetImpl()->SetBroadcast(bFlag);
	}

	BOOL RawSocket::IsBroadcast()
	{
		return GetImpl()->IsBroadcast();
	}
}