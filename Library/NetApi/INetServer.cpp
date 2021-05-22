#include "StdAfx.h"
#include "INetServer.h"

INetServer::INetServer()
	: m_pfnConcb(NULL)
{

}

INetServer::~INetServer()
{

}

void INetServer::Release()
{
	delete this;
}

const uv_loop_t* INetServer::GetLoop() const
{
	return m_pLoop;
}

void INetServer::SetConnectExtra(FN_Connect cb)
{
	m_pfnConcb = cb;
}

LPVOID INetServer::GetInterface(NETSERVER_TYPE ServerType)
{
	if ( ServerType == NETSERVER_TYPE_BASE) return this;
	return NULL;
}

INetServer* INetServer::CreateServer(NETSERVER_TYPE ServerType, uv_loop_t* loop/* = uv_default_loop()*/)
{
	INetServer* pNetServer = NULL;
	switch(ServerType)
	{
		case NETSERVER_TYPE_TCP:
				pNetServer = new CTCPServer;
			break;
		case NETSERVER_TYPE_UDP:
				pNetServer = new CUDPServer;
			break;
	}

	pNetServer->m_pLoop = loop;

	return pNetServer;
}