#include "Stdafx.h"
#include "SocketImpl.h"

BOOL CheckIsBrokenTimeout()
{
	// on Windows 7 and lower, socket timeouts have a minimum of 500ms, use poll for timeouts on this case
	// https://social.msdn.microsoft.com/Forums/en-US/76620f6d-22b1-4872-aaf0-833204f3f867/minimum-timeout-value-for-sorcvtimeo
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(vi);
	if (GetVersionEx(&vi) == 0) return true; //throw SystemException("Cannot get OS version information");
		return vi.dwMajorVersion < 6 || (vi.dwMajorVersion == 6 && vi.dwMinorVersion < 2);
}

namespace NetCore
{
	SocketImpl::SocketImpl()
	: m_sockfd(POCO_INVALID_SOCKET)
	, m_bLocking(TRUE)
	{
		m_bBrokenTimeout = CheckIsBrokenTimeout();
	}

	SocketImpl::SocketImpl(poco_socket_t sockfd)
		: m_sockfd(sockfd)
		, m_bLocking(TRUE)
	{
		m_bBrokenTimeout = CheckIsBrokenTimeout();
	}

	SocketImpl::~SocketImpl()
	{
		Close();
	}

	/*SocketImpl* SocketImpl::AcceptConnection(SocketAddress& ClientAddr)
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return NULL;
		
		BYTE bData[MAX_ADDRESS_LENGTH];
		SOCKADDR* pSA = reinterpret_cast<struct sockaddr*>(bData);

		poco_socket_t sd;
		poco_socklen_t saLen = sizeof(bData);

		do 
		{
			sd = ::accept(m_sockfd, pSA, &saLen);
		} while (sd == POCO_INVALID_SOCKET && GetLastError() == POCO_EINTR);

		if (sd != POCO_INVALID_SOCKET)
		{
			ClientAddr = SocketAddress(pSA, saLen);
			return new StreamSocketImpl(sd);
		}

		return NULL;	
	}*/

	SocketImpl* SocketImpl::AcceptConnection(SocketAddress& ClientAddr)
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return NULL;
		char buffer[MAX_ADDRESS_LENGTH];
		SOCKADDR* pSA = reinterpret_cast<SOCKADDR*>(buffer);
		poco_socklen_t saLen = sizeof(buffer);
		poco_socket_t sd;
		do 
		{
			sd = ::accept(m_sockfd, pSA, &saLen);
		} while (sd == POCO_INVALID_SOCKET && GetLastError() == POCO_EINTR);

		if (sd != POCO_INVALID_SOCKET)
		{
			ClientAddr = SocketAddress(pSA, saLen);
			return new StreamSocketImpl(sd);
		}

		return NULL;
	}

	void SocketImpl::Connect(const SocketAddress& Address)
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			Init(Address.GetAF());

		int rc = ::connect(m_sockfd, Address.GetAddr(), Address.GetLen());
		while (rc != 0 && GetLastError() == POCO_EINTR);
	}

	//void SocketImpl::Connect(const SocketAddress& Address)

	void SocketImpl::ConnectNB(const SocketAddress& Address)
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			Init(Address.GetAF());

		SetBlocking(FALSE);

		int rc = ::connect(m_sockfd, Address.GetAddr(), Address.GetLen());
	}

	void SocketImpl::Bind(const SocketAddress& Address, BOOL bReuseAddress, BOOL bReusePort )
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			Init(Address.GetAF());

		if (bReuseAddress)
			SetReuseAddress(TRUE);
		if (bReusePort)
			SetReusePort(TRUE);

		int rc = ::bind(m_sockfd, Address.GetAddr(), Address.GetLen());
	}

	void SocketImpl::Bind(const SocketAddress& Address, BOOL bReuseAddress /* = FALSE */)
	{
		Bind(Address, bReuseAddress, TRUE);	
	}

	void SocketImpl::Bind6(const SocketAddress& Address, BOOL bReuseAddress, BOOL bReusePort, BOOL bIPV6Only)
	{
		if (Address.GetFamily() != SocketAddress::IPv6)
			return;

		if (m_sockfd == POCO_INVALID_SOCKET)
			Init(Address.GetAF());

#ifdef IPV6_V6ONLY
		//	需要定义_WIN32_WINNT 0x0501 才能正常使用
		SetOption(IPPROTO_IPV6, IPV6_V6ONLY, bIPV6Only ? TRUE : FALSE);
#endif

		if (bReuseAddress)
			SetReuseAddress(TRUE);
		if (bReusePort)
			SetReusePort(TRUE);
		int nRet = ::bind(m_sockfd, Address.GetAddr(), Address.GetLen());
	}

	void SocketImpl::Bind6(const SocketAddress& Address, BOOL bReuseAddress /* = FALSE */, BOOL bIPV6Only /* = FALSE */)
	{
		Bind6(Address, bReuseAddress, TRUE, bIPV6Only);
	}

	void SocketImpl::Listen(int nBacklog /* = 64 */)
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return;
		int nRet = ::listen(m_sockfd, nBacklog);
	}

	void SocketImpl::Close()
	{
		if (m_sockfd != POCO_INVALID_SOCKET)
		{
			poco_closesocket(m_sockfd);
			m_sockfd = POCO_INVALID_SOCKET;
		}
	}

	void SocketImpl::ShutdownReceive()
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return;

		int nRet = ::shutdown(m_sockfd, 0);
	}

	void SocketImpl::ShutdownSend()
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return;

		int nRet = ::shutdown(m_sockfd, 1);	
	}

	void SocketImpl::Shutdown()
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return;

		int nRet = ::shutdown(m_sockfd, 2);	
	}

	int SocketImpl::SendBytes(const void* pBbuffer, int nLen, int nFlags /* = 0 */)
	{
		if (m_bBrokenTimeout)
		{
			/*if (_recvTimeout.totalMicroseconds() != 0)
			{
				if (!poll(_recvTimeout, SELECT_READ))
					throw TimeoutException();
			}*/
			ASSERT(FALSE);
		}

		int nRet  = 0;
		do 
		{
			if (m_sockfd == POCO_INVALID_SOCKET)
				break;
			const char* p = reinterpret_cast<const char*>(pBbuffer);
			nRet = ::send(m_sockfd, p, nLen, nFlags);
		} while (m_bLocking && nRet < 0 && GetLastError() == POCO_EINTR);
		
		return nRet;
	}

	int SocketImpl::ReceiveBytes(void* pBuffer, int nLen, int nFlags /* = 0 */)
	{
		if (m_bBrokenTimeout)
		{
			ASSERT(FALSE);
		}

		int nRet = 0;
		do 
		{
			if (m_sockfd == POCO_INVALID_SOCKET)
				break;
			char* p = reinterpret_cast<char*>(pBuffer);
			nRet = ::recv(m_sockfd, p , nLen, nFlags);
		} while (m_bLocking && nRet < 0 && GetLastError() == POCO_EINTR);
		
		/*if (nRet < 0) 
		{
			int err = lastError();
			if (err == POCO_EAGAIN && !_blocking)
				;
			else if (err == POCO_EAGAIN || err == POCO_ETIMEDOUT)
				throw TimeoutException(err);
			else
				error(err);
		}*/
		
		return nRet;
	}

	int SocketImpl::SendTo(const void* pBuffer, int nLen, const SocketAddress& Address, int nFlags /* = 0 */)
	{
		int nRet = 0;
		do 
		{
			if (m_sockfd == POCO_INVALID_SOCKET)
				break;
			const char* p = reinterpret_cast<const char*>(pBuffer);
			nRet = ::sendto(m_sockfd, p ,nLen, nFlags, Address.GetAddr(), Address.GetLen());
		} while (m_bLocking && nRet < 0 && GetLastError() == POCO_EINTR);
		
		return nRet;
	}

	int SocketImpl::ReceiveFrom(void* pBuffer, int nLen, SocketAddress& Address, int nFlags /* = 0 */)
	{
		if (m_bBrokenTimeout)
		{
			/*if (_recvTimeout.totalMicroseconds() != 0)
			{
				if (!poll(_recvTimeout, SELECT_READ))
					throw TimeoutException();
			}*/
		}

		char buffer[MAX_ADDRESS_LENGTH];
		SOCKADDR* pSA = reinterpret_cast<SOCKADDR*>(buffer);
		poco_socklen_t saLen = sizeof(buffer);
		
		int nRet = 0;
		do 
		{
			if (m_sockfd == POCO_INVALID_SOCKET)
				break;

			char* p = reinterpret_cast<char*>(pBuffer);
			nRet = ::recvfrom(m_sockfd, p, nLen, nFlags, pSA, &nLen);
		} while (m_bLocking && nRet < 0 && GetLastError() == POCO_EINTR);
		
		if (nRet > 0)
			Address = SocketAddress(pSA, saLen);
		return nRet;
	}

	void SocketImpl::SendUrgent(unsigned char data)
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return;

		const char* p = reinterpret_cast<const char*>(&data);
		int nRet = ::send(m_sockfd, p, sizeof(data), MSG_OOB);
		//return nRet;
	}

	int SocketImpl::GetAvailable()
	{
		int nRet;
		IOCTL(FIONREAD, nRet);
		return nRet;
	}

	BOOL SocketImpl::IsSecure() const
	{
		return FALSE;
	}

	int SocketImpl::GetSockFD() const
	{
		return m_sockfd;
	}

	void SocketImpl::SetSendBufferSize(int size)
	{
		SetOption(SOL_SOCKET, SO_SNDBUF, size);
	}

	int SocketImpl::GetSendBufferSize()
	{
		int nRet;
		GetOption(SOL_SOCKET, SO_SNDBUF, nRet);
		return nRet;
	}

	void SocketImpl::SetReceiveBufferSize(int size)
	{
		SetOption(SOL_SOCKET, SO_RCVBUF, size);
	}

	int SocketImpl::GetReceiveBufferSize()
	{
		int nRet;
		GetOption(SOL_SOCKET, SO_RCVBUF, nRet);
		return nRet;
	}

	SocketAddress SocketImpl::GetAddress()
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return SocketAddress();

		char buffer[MAX_ADDRESS_LENGTH];
		SOCKADDR* pSA = reinterpret_cast<SOCKADDR*>(buffer);
		poco_socklen_t saLen = sizeof(buffer);
		int nRet = ::getsockname(m_sockfd, pSA, &saLen);
		if (nRet == 0)
			return SocketAddress(pSA, saLen);
		return SocketAddress();
	}

	SocketAddress SocketImpl::GetPeerAddress()
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return SocketAddress();

		char buffer[MAX_ADDRESS_LENGTH];
		SOCKADDR* pSA = reinterpret_cast<SOCKADDR*>(buffer);

		poco_socklen_t saLen = sizeof(buffer);
		int nRet = ::getpeername(m_sockfd, pSA, &saLen);

		if (nRet == 0)
			return SocketAddress(pSA, saLen);

		return SocketAddress();
	}

	void SocketImpl::SetOption(int level, int option, int value)
	{
		SetRawOption(level, option, &value, sizeof(value));
	}

	void SocketImpl::SetRawOption(int level, int option, const void* value, int nLen)
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return;

		const char* p = reinterpret_cast<const char*>(value);
		int nRet = ::setsockopt(m_sockfd, level, option, p, nLen);
	}

	void SocketImpl::GetOption(int level, int option, int& value)
	{
		poco_socklen_t nLen = sizeof(value);
		GetRawOption(level, option, &value, nLen);
	}

	void SocketImpl::GetRawOption(int level, int option, void* value, int& nLen)
	{
		if (m_sockfd == POCO_INVALID_SOCKET)
			return;

		char* p = reinterpret_cast<char*>(value);
		int nRet = ::getsockopt(m_sockfd, level, option, p , &nLen);
	}

	void SocketImpl::SetLinger(BOOL bLinger, int nSeconds)
	{
		linger l;
		l.l_onoff = bLinger ? TRUE : FALSE;
		l.l_linger = nSeconds;

		SetRawOption(SOL_SOCKET, SO_LINGER, &l, sizeof(linger));
	}

	void SocketImpl::GetLinger(BOOL& bLinger, int& nSeconds)
	{
		linger l;
		poco_socklen_t nLen = sizeof(linger);
		GetRawOption(SOL_SOCKET, SO_LINGER, &l, nLen);
		bLinger = l.l_onoff != FALSE;
		nSeconds = l.l_linger;
	}

	void SocketImpl::SetNoDelay(BOOL bFlag)
	{
		SetOption(IPPROTO_TCP, TCP_NODELAY, bFlag);
	}

	BOOL SocketImpl::IsNoDelay()
	{
		BOOL bNoDelay = FALSE;
		GetOption(IPPROTO_TCP, TCP_NODELAY, bNoDelay);

		return bNoDelay;
	}

	void SocketImpl::SetKeepAlive(BOOL bFlag)
	{
		SetOption(SOL_SOCKET, SO_KEEPALIVE, bFlag);
	}

	BOOL SocketImpl::IsKeepAlive()
	{
		BOOL bKeepAlive = FALSE;
		GetOption(SOL_SOCKET, SO_KEEPALIVE, bKeepAlive);
		return bKeepAlive;
	}

	void SocketImpl::SetReuseAddress(BOOL bFlag)
	{
		SetOption(SOL_SOCKET, SO_REUSEADDR, bFlag);
	}

	BOOL SocketImpl::IsReuseAddress()
	{
		BOOL bReuseAddr = FALSE;
		GetOption(SOL_SOCKET, SO_REUSEADDR, bReuseAddr);
		return bReuseAddr;
	}

	void SocketImpl::SetReusePort(BOOL bFlag)
	{
		//SetOption(SOL_SOCKET, SO_REUSEPORT)
	}

	BOOL SocketImpl::IsReusePort()
	{
		return FALSE;
	}

	void SocketImpl::SetBroadcast(BOOL bFlag)
	{
		SetOption(SOL_SOCKET, SO_BROADCAST, bFlag);
	}

	BOOL SocketImpl::IsBroadcast()
	{
		BOOL bBroadcast = FALSE;
		GetOption(SOL_SOCKET, SO_BROADCAST, bBroadcast);
		return bBroadcast;
	}

	void SocketImpl::SetBlocking(BOOL bFlag)
	{
		int nArgc = bFlag ? FALSE : TRUE;
		IOCTL(FIONBIO, nArgc);

		m_bLocking = bFlag;
	}

	BOOL SocketImpl::IsBlocking() const
	{
		return m_bLocking;
	}

	void SocketImpl::Init(int AF)
	{
		InitSocket(AF, SOCK_STREAM);
	}

	void SocketImpl::InitSocket(int AF, int nType, int nProto /* = 0 */)
	{
		if (m_sockfd != POCO_INVALID_SOCKET)
			return ;

		m_sockfd = ::socket(AF, nType, nProto);
		if (m_sockfd == POCO_INVALID_SOCKET)
			return;
	}

	void SocketImpl::IOCTL(poco_ioctl_request nRequest, int& arg)
	{
		int nRet = ::ioctlsocket(m_sockfd, nRequest,(u_long*)&arg);
	}

	void SocketImpl::IOCTL(poco_ioctl_request nRequest, void* arg)
	{
		int nRet = ::ioctlsocket(m_sockfd, nRequest, (u_long*)arg);
	}

	void SocketImpl::Reset(poco_socket_t fd /* = INVALID_SOCKET */)
	{
		m_sockfd = fd;
	}

	BOOL SocketImpl::IsInitialized() const
	{
		return m_sockfd != POCO_INVALID_SOCKET;
	}

	DWORD SocketImpl::GetLastError()
	{
		return WSAGetLastError();
	}
}