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

//CTCPServer* pTCPServer = NULL;
//
//class CTCPHandlerEx : public CTCPHandler
//{
//public:
//	CTCPHandlerEx(){};
//	~CTCPHandlerEx(){};
//public:
//	virtual void Init(){CTCPHandler::Init();}
//	virtual void OnClose(){CTCPHandler::OnClose();}
//	virtual void Recv(ISession* pSession, LPCSTR lpszData, int nLen){
//		CTCPServer* pServer = (CTCPServer*)pTCPServer->GetInterface(NETSERVER_TYPE_TCP);
//		pSession->Send(lpszData, nLen);
//	}
//};
//
//BOOL OnConnectCB(size_t nClientId)
//{ 
//	IHandler* pTCPHandler = new CTCPHandlerEx;
//	pTCPHandler->Init();
//	//设置新连接接收数据回调函数 
//	pTCPServer->SetHandlerCB(nClientId, pTCPHandler);
//
//	return TRUE;
//}
//
//int main()
//{
//	uv_loop_t* loop = uv_default_loop();
//	INetServer* pServer = INetServer::CreateServer(NETSERVER_TYPE_TCP, loop);
//	if (pServer != NULL)
//		pTCPServer = (CTCPServer*)pServer->GetInterface(NETSERVER_TYPE_TCP);
//	pTCPServer->SetConnectExtra(OnConnectCB);
//	pTCPServer->SetAllocByte(NONE_ALLOC_BUFFER);
//	pTCPServer->Init(_T("10.1.1.163"), 8080, 5);
//
//	pTCPServer->Start(UV_RUN_DEFAULT);
//	return 0;
//}