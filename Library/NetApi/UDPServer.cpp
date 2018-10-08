#include "StdAfx.h"
#include "UDPServer.h"

CUDPServer::CUDPServer()
{

}

CUDPServer::~CUDPServer()
{

}

BOOL CUDPServer::Init(LPCTSTR lpszServerAddr, WORD wPort, UINT nMax)
{
	return FALSE;
}

void CUDPServer::Start(UV_RUN_MODE uvMode)
{
	uv_run(m_pLoop, uvMode);
}

void CUDPServer::Shutdown()
{
	uv_close((uv_handle_t*)&m_udp_server, NULL);
}

LPVOID CUDPServer::GetInterface(NETSERVER_TYPE ServerType /*= NETSERVER_TYPE_BASE*/)
{
	if ( ServerType == NETSERVER_TYPE_UDP) return this;
	else
		return INetServer::GetInterface(ServerType);
}

void CUDPServer::SetHandlerCB(size_t nClientId, IHandler* pHandler)
{

}