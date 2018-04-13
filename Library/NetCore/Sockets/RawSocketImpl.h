#pragma once

namespace NetCore
{
	class NET_API RawSocketImpl : public SocketImpl
	{
	public:
		RawSocketImpl();
		RawSocketImpl(Family family, int nProto = IPPROTO_RAW);
		RawSocketImpl(poco_socket_t sockfd);

	protected:
		void Init(int AF);
		void Init(int AF, int nProto);

	protected:
		~RawSocketImpl();
	};
}