#include "Stdafx.h"
#include "Socket.h"

namespace NetCore
{
	Socket::Socket()
		: m_pImpl(new StreamSocketImpl)
	{

	}

	Socket::Socket(SocketImpl* pImpl)
		: m_pImpl(pImpl)
	{

	}

	Socket::Socket(const Socket& socket)
		: m_pImpl(socket.GetImpl())
	{
		m_pImpl->Duplicate();
	}

	Socket::~Socket()
	{
		m_pImpl->Release();
	}

	Socket& Socket::operator =(const Socket& socket)
	{
		if (&socket != this)
		{
			if (m_pImpl)
				m_pImpl->Release();
			m_pImpl = socket.GetImpl();
			if (m_pImpl)
				m_pImpl->Duplicate();
		}

		return *this;
	}

	void Socket::Close()
	{
		m_pImpl->Close();
	}

	BOOL Socket::GetAvailable() const
	{
		return m_pImpl->GetAvailable();
	}

	void Socket::SetSendBufferSize(int size)
	{
		m_pImpl->SetSendBufferSize(size);
	}

	int Socket::GetSendBufferSize() const
	{
		return m_pImpl->GetSendBufferSize();
	}

	void Socket::SetReceiveBufferSize(int size)
	{
		m_pImpl->SetReceiveBufferSize(size);
	}

	int Socket::GetReceiveBufferSize() const
	{
		return m_pImpl->GetReceiveBufferSize();
	}

	void Socket::SetOption(int level, int option, int value)
	{
		m_pImpl->SetOption(level, option, value);
	}

	void Socket::GetOption(int level, int option, int& value) const
	{
		m_pImpl->GetOption(level, option, value);
	}

	void Socket::SetLinger(BOOL on, int seconds)
	{
		m_pImpl->SetLinger(on, seconds);
	}

	void Socket::GetLinger(BOOL& on, int& seconds) const
	{
		m_pImpl->GetLinger(on,seconds);
	}

	void Socket::SetNoDelay(BOOL flag)
	{
		m_pImpl->SetNoDelay(false);
	}

	BOOL Socket::IsNoDelay() const
	{
		return m_pImpl->IsNoDelay();
	}

	void Socket::SetKeepAlive(BOOL flag)
	{
		m_pImpl->SetKeepAlive(flag);
	}

	BOOL Socket::IsKeepAlive() const
	{
		return m_pImpl->IsKeepAlive();
	}

	void Socket::SetReuseAddress(BOOL flag)
	{
		m_pImpl->SetReuseAddress(flag);
	}

	BOOL Socket::IsReuseAddress() const
	{
		return m_pImpl->IsReuseAddress();
	}

	void Socket::SetReusePort(BOOL flag)
	{
		m_pImpl->SetReusePort(flag);
	}

	BOOL Socket::IsReusePort() const
	{
		return m_pImpl->IsReusePort();
	}

	void Socket::SetBlocking(BOOL flag)
	{
		m_pImpl->SetBlocking(flag);
	}

	BOOL Socket::IsBlocking() const
	{
		return m_pImpl->IsBlocking();
	}

	void Socket::Init(int AF)
	{
		m_pImpl->Init(AF);
	}
	
	SocketImpl* Socket::GetImpl() const
	{
		return m_pImpl;
	}

	poco_socket_t Socket::GetSocketFD() const
	{
		return m_pImpl->GetSockFD();
	}

	SocketAddress Socket::GetAddress() const
	{
		return m_pImpl->GetAddress();
	}

	SocketAddress Socket::GetPeerAddress() const
	{
		return m_pImpl->GetPeerAddress();
	}

	BOOL Socket::IsSecure() const
	{
		return m_pImpl->IsSecure();
	}

	BOOL Socket::SupportsIPv4()
	{
		return TRUE;
	}

	BOOL Socket::SupportsIPv6()
	{
#ifdef POCO_HAVE_IPv6
		return TRUE;
#else
		return FALSE;
#endif
	}
}