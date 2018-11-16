#pragma once

#ifdef LS_STATIC_LIB_CALL
#define NET_API 
#elif defined  NETCORE_EXPORTS
#define NET_API __declspec(dllexport)
#else
#define NET_API __declspec(dllimport)
#endif

#include <WinSock2.h>
#include <ws2def.h>
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <vector>
#include <list>
#include <map>
#include <Iphlpapi.h>

#include "Utils/ListMap.h"
#include "Utils/NameValueCollection.h"

#include "Sockets/Define.h"
#include "Utils/RefSocketObject.h"
#include "Sockets/IPAddressImpl.h"
#include "Sockets/IPAddress.h"
#include "Utils/HostEntry.h"
#include "Utils/DNS.h"
#include "Utils/NetworkInterface.h"
#include "Sockets/SocketAddressImpl.h"
#include "Sockets/SocketAddress.h"

#include "Sockets/SocketImpl.h"
#include "Sockets/RawSocketImpl.h"
#include "Sockets/StreamSocketImpl.h"
#include "Sockets/DatagramSocketImpl.h"

#include "Sockets/Socket.h"
#include "Sockets/RawSocket.h"
#include "Sockets/StreamSocket.h"
#include "Sockets/DatagramSocket.h"

#include "HTTP/MediaType.h"
#include "HTTP/MessageHeader.h"
#include "HTTP/HTTPMessage.h"

#include "WebSocket/WebSocketImpl.h"
#include "WebSocket/WebSocket.h"