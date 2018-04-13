#include "Stdafx.h"
#include "StreamSocketImpl.h"

namespace NetCore
{
	StreamSocketImpl::StreamSocketImpl()
	{

	}

	StreamSocketImpl::StreamSocketImpl(Family AddressFamily)
	{
		if (AddressFamily == SocketAddress::IPv4)
			Init(AF_INET);
		else if (AddressFamily == SocketAddress::IPv6)
			Init(AF_INET6);
	}

	StreamSocketImpl::StreamSocketImpl(poco_socket_t sockfd)
		: SocketImpl(sockfd)
	{

	}

	StreamSocketImpl::~StreamSocketImpl()
	{

	}

	int StreamSocketImpl::SendBytes(const void* pBbuffer, int nLen, int nFlags )
	{
		int nSendBytes = 0;
		int nRemain = nLen;
		const char* p = reinterpret_cast<const char*>(pBbuffer);
		
		BOOL bLocking = IsBlocking();
		while(nRemain > 0)
		{
			int nSend = SocketImpl::SendBytes(pBbuffer, nRemain, nFlags);
			p += nSend;
			nSendBytes += nSend;
			nRemain -= nSend;

			if (bLocking && nRemain > 0)
				Sleep(10);
			else
				break;
		}
		return nSendBytes;
	}
}