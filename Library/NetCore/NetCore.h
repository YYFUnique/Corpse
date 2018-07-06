#pragma once

#ifdef LS_STATIC_LIB_CALL
#define NET_API 
#elif defined  NETCORE_EXPORTS
#define NET_API __declspec(dllexport)
#else
#define NET_API __declspec(dllimport)
#endif

#include "Utils/ListMap.h"
#include "Utils/NameValueCollection.h"

#include "Sockets/Define.h"
#include "Utils/RefSocketObject.h"
#include "Sockets/IPAddressImpl.h"
#include "Sockets/IPAddress.h"
#include "Utils/HostEntry.h"
#include "Utils/DNS.h"
#include "Sockets/SocketAddressImpl.h"
#include "Sockets/SocketAddress.h"

#include "Sockets/SocketImpl.h"
#include "Sockets/RawSocketImpl.h"
#include "Sockets/StreamSocketImpl.h"
#include "Sockets/DatagramSocketImpl.h"

#include "Sockets/Socket.h"
#include "Sockets/RawSocket.h"
#include "Sockets/DatagramSocket.h"
