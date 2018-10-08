#pragma once

#ifdef LS_STATIC_LIB_CALL
#define NETSERVER_API 
#elif defined  NETSERVER_EXPORTS
#define NETSERVER_API __declspec(dllexport)
#else
#define NETSERVER_API __declspec(dllimport)
#endif

#include <atlstr.h>
#include <map>
#include <libuv/uv.h>
#include <DllCore/Log/LogHelper.h>


#include "IHandler.h"
#include "TCPHandler.h"
#include "INetServer.h"
#include "TCPServer.h"
#include "UDPServer.h"
#include "Session.h"