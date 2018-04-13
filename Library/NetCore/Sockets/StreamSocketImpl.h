#pragma once

namespace NetCore
{
	class NET_API StreamSocketImpl : public SocketImpl
	{
	public:
		StreamSocketImpl();

		explicit StreamSocketImpl(Family AddressFamily);

		StreamSocketImpl(poco_socket_t sockfd);

		virtual int SendBytes(const void* pBbuffer, int nLen, int nFlags /* = 0 */);

	protected:
		virtual ~StreamSocketImpl();
	};
}