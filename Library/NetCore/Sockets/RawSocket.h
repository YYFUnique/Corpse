#pragma once

namespace NetCore
{
	class NET_API RawSocket : public Socket
	{
	public:
		RawSocket();
		RawSocket(Family family, int nProto = IPPROTO_RAW);
		RawSocket(const SocketAddress& ADDR, BOOL bReuseAddr = FALSE);
		RawSocket(const Socket& socket);

		~RawSocket();
	public:
		RawSocket& operator = (const Socket& socket);
	public:
		void Connect(const SocketAddress& Addr);
		void Bind(const SocketAddress& Addr, BOOL bReuseAddr = FALSE);
		void Bind(const SocketAddress& Addr, BOOL bReuseAddr, BOOL bReusePort);
		int SendBytes(const void* pBuffer, int nLen, int nFlag = 0);
		int  ReceiveBytes(void* pBuffer, int nLen, int nFalg = 0);
		int SendTo(const void* pBuffer, int nLen, const SocketAddress& Addr, int nFlag =0);
		int ReceiveFrom(void* pBuffer, int nLen, SocketAddress& Addr, int nFlag = 0);
		void SetBroadcast(BOOL bFlag);
		BOOL IsBroadcast();
	protected:
		RawSocket(SocketImpl* pImpl);
	};
}