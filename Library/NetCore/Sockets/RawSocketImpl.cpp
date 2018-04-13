#include "StdAfx.h"
#include "RawSocketImpl.h"

namespace NetCore
{
	RawSocketImpl::RawSocketImpl()
	{
		Init(AF_INET);
	}

	RawSocketImpl::RawSocketImpl(Family family, int nProto /* = IPPROTO_RAW */)
	{
		if (family == IPv4)
			Init(AF_INET, nProto);
		else if (family == IPv6)
			Init(AF_INET6, nProto);
	}

	RawSocketImpl::RawSocketImpl(poco_socket_t sockfd)
		: SocketImpl(sockfd)
	{

	}

	RawSocketImpl::~RawSocketImpl()
	{

	}

	void RawSocketImpl::Init(int AF)
	{
		Init(AF, IPPROTO_RAW);
	}

	void RawSocketImpl::Init(int AF, int nProto)
	{
		InitSocket(AF, SOCK_RAW, nProto);
		SetOption(IPPROTO_IP, IP_HDRINCL, 0);
	}
}