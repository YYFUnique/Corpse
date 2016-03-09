#pragma once

#ifdef LS_STATIC_LIB_CALL
#define NET_TRANSPORT_API
#elif defined LS_EXPORTS
#define NET_TRANSPORT_API __declspec(dllexport)
#else
#define NET_TRANSPORT_API __declspec(dllimport)
#endif

NET_TRANSPORT_API BOOL GetRemoteInfo(SOCKET soClient,CString& strRemoteIp,WORD& wPort);