#include "StdAfx.h"
#include "TCPServer.h"

CTCPServer::CTCPServer()
{
	m_dwAllocByte = DEFAULT_ALLOC_BUFFER;
}

CTCPServer::~CTCPServer()
{
	
}

void CTCPServer::SetAllocByte(DWORD dwAllocByte)
{
	m_dwAllocByte = dwAllocByte;
}

DWORD CTCPServer::GetAllocByte() const
{
	return m_dwAllocByte;
}

void CTCPServer::Start(UV_RUN_MODE uvMode/* = UV_RUN_DEFAULT*/)
{
	uv_run(m_pLoop, uvMode);
}

void CTCPServer::Shutdown()
{
	uv_close((uv_handle_t*)&m_tcp_server, NULL);
}

LPVOID CTCPServer::GetInterface(NETSERVER_TYPE ServerType /*= NETSERVER_TYPE_BASE*/)
{
	if ( ServerType == NETSERVER_TYPE_TCP) return this;
	else
		return INetServer::GetInterface(ServerType);
}

void CTCPServer::SetHandlerCB(size_t nClientId, IHandler* pTCPHandler)
{
	ClientConnMap::iterator itFind = ClientObjMap.find(nClientId);
	if (itFind != ClientObjMap.end())
		itFind->second->SetHandler(pTCPHandler);
}

int CTCPServer::Send(size_t ClientId, LPCSTR lpszData, size_t nLen, BOOL bSend/* = TRUE*/)
{
	//服务器发送数据给客户端
	ClientConnMap::iterator itFind = ClientObjMap.find(ClientId);

	//找不到指定ID的客户端
	if (itFind == ClientObjMap.end())
		return -1;

	return itFind->second->Send(lpszData, nLen);
}

BOOL CTCPServer::Init(LPCTSTR lpszServerAddr, WORD wPort, UINT nMax)
{
	BOOL bSuccess = FALSE;
	do 
	{
		uv_stream_t* uvStream = NULL;
		SOCKADDR_IN InAddr;
		ASSERT(uv_ip4_addr(CStringA(lpszServerAddr), wPort, &InAddr) == 0);

		int nRet = uv_tcp_init(m_pLoop, &m_tcp_server);
		if (nRet != NO_ERROR)
			break;

		nRet = uv_tcp_bind(&m_tcp_server, (const SOCKADDR*)&InAddr, 0);
		if (nRet != NO_ERROR)
			break;

		uvStream = (uv_stream_t*)&m_tcp_server;
		nRet = uv_listen(uvStream, nMax, OnConnect);
		if (nRet != NO_ERROR)
			break;

		uvStream->data = this;
		
		bSuccess = TRUE;
	} while (FALSE);
	
	return bSuccess;
}

size_t CTCPServer::GetAvailaClientID()
{
	static size_t nClientCount = 0;
	return ++nClientCount;
}

void CTCPServer::OnConnect(uv_stream_t* uvStream, int nStatus)
{
	ASSERT(nStatus == 0);
	

	BOOL bSuccess = FALSE;
	CSessionData* pClientData = NULL;

	CTCPServer* pTCPServer = (CTCPServer*)uvStream->data;
	size_t nClientId = pTCPServer->GetAvailaClientID();
	do 
	{
		pClientData = new CSessionData(nClientId);
		pClientData->SetServer(pTCPServer);
		uv_stream_t* pStream = pClientData->GetStream();
		int nRet = uv_tcp_init(pTCPServer->m_pLoop, (uv_tcp_t*)pStream);
		if (nRet != NO_ERROR)
			break;

		nRet = uv_accept((uv_stream_t*)&pTCPServer->m_tcp_server, pStream);
		if (nRet != NO_ERROR)
			break;

		// 加入到链接队列中
		pTCPServer->ClientObjMap.insert(std::make_pair(nClientId, pClientData));
		ASSERT(pTCPServer->m_pfnConcb);
		if (pTCPServer->m_pfnConcb(nClientId) == FALSE)
			break;

		nRet = uv_read_start(pStream, OnAllocBuffer, AfterServerRecv);
		if (nRet != NO_ERROR)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
		pTCPServer->DeleteClient(pClientData, nClientId);
}

void CTCPServer::DeleteClient(CSessionData* pClientData, size_t nClientId)
{
	if (pClientData != NULL)
		pClientData->Close();

	ClientConnMap::iterator itFind = ClientObjMap.find(nClientId);
	if (itFind == ClientObjMap.end())
		return;

	ClientObjMap.erase(itFind);
	return;
}

void CTCPServer::AfterServerRecv(uv_stream_t* pStream, ssize_t nRead, const uv_buf_t* buf)
{
	CSessionData* pClientData = (CSessionData*)pStream->data;

	do 
	{
		CTCPServer* pServer = (CTCPServer*)pClientData->GetServer()->GetInterface(NETSERVER_TYPE_TCP);
		// 目前未区分读取错误具体情况
		if (nRead < 0)
		{
			if (pServer != NULL)
				pServer->DeleteClient(pClientData, pClientData->GetClientId());
			LOG(_T("AfterServerRecv error: %d"), nRead);
			
			break;
		}
		// 读取到内容
		else if (nRead > 0)
		{
			// 将接收的内容缓存起来，等全部接收完成后继续处理
			pClientData->OnMessage(buf->base, nRead);
			ssize_t sAllocByte = pServer->GetAllocByte();
			// 如果sAllocByte为-1
			if (nRead < sAllocByte || sAllocByte == NONE_ALLOC_BUFFER)
				pClientData->OnRecv();
		}
		else 
			pClientData->OnRecv();
	} while (FALSE);
	
	free(buf->base);
}

void CTCPServer::OnAllocBuffer(uv_handle_t *uvhClient, size_t dwSize, uv_buf_t *ReadBuf)
{
	CSessionData* pClientData = (CSessionData*)uvhClient->data;
	CTCPServer* pTCPServer = (CTCPServer*)pClientData->GetServer()->GetInterface(NETSERVER_TYPE_TCP);

	// 如果默认分配缓冲区大小为-1，使用函数形参，否则使用内部值
	size_t dwAllocByte = pTCPServer->GetAllocByte();
	if (dwAllocByte == NONE_ALLOC_BUFFER)
		dwAllocByte = dwSize;

	ReadBuf->base = (char*) malloc(dwAllocByte); //堆上创建buff
	if (ReadBuf->base != NULL)
		ReadBuf->len = dwAllocByte;
}