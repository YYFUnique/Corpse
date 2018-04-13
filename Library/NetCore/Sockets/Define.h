#pragma once

namespace NetCore
{
	#define poco_socket_t SOCKET
	#define poco_socklen_t int
	#define poco_ioctl_request int
	#define POCO_INVALID_SOCKET  INVALID_SOCKET
	#define poco_closesocket(s)  closesocket(s)
	#define POCO_EINTR           WSAEINTR

#if defined(POCO_USE_IPv6)
	static const int MAX_ADDRESS_LENGTH = sizeof(SOCKADDR_IN6);
#else
	static const int MAX_ADDRESS_LENGTH = sizeof(SOCKADDR_IN);
#endif


	/// The mode argument to poll() and select().
	typedef enum tagSelectMode
	{
		SELECT_MODE_READ		= 1,
		SELECT_MODE_WRITE		= 2,
		SELECT_MODE_ERROR	= 4
	}SELECT_MODE;

	typedef struct tagAddressFamily
	{
		typedef enum tagFamily
		{
			IPv4,
			IPv6,
		}Family;
	}AddressFamily;

#ifndef Family
	typedef AddressFamily::Family Family;
#endif
}