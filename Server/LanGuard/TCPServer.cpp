#include "StdAfx.h"
#include "TCPServer.h"
#include <signal.h>

CTCPServer::CTCPServer(uv_loop_t* loop /* = uv_default_loop */)
: m_pNewConcb(NULL)
, m_bInit(FALSE)
{
	m_uvpLoop = loop;
	//m_uvhMutex = NULL;
}

CTCPServer::~CTCPServer()
{
	Close();
}

BOOL CTCPServer::Init()
{
	if (m_bInit)
		return TRUE;

	if (m_uvpLoop == NULL)
		return FALSE;

	int nRet = uv_mutex_init(&m_uvhMutex);
	if (nRet)
		return FALSE;
	
	nRet = uv_tcp_init(m_uvpLoop, &m_uvServer);
	if (nRet)
		return FALSE;

	m_bInit = TRUE;
	m_uvServer.data = this;

	return TRUE;
}

void CTCPServer::Close()
{
	//关闭服务器
	for (ClientConnMap::iterator it = ClientObjMap.begin(); it != ClientObjMap.end();++it)
	{
		ClientData* pClientData = (ClientData*)it->second;
		uv_close((uv_handle_t*)pClientData->m_uvhClient, AfterClientClose);
	}

	ClientObjMap.clear();
	if (m_bInit)
		uv_close((uv_handle_t*)&m_uvServer,AfterServerClose);

	m_bInit = FALSE;
	uv_mutex_destroy(&m_uvhMutex);
}

BOOL CTCPServer::SetNoDelay(BOOL bDelay /*= FALSE*/)
{
	//属性设置---服务器与客户端一致
	int nRet = uv_tcp_nodelay(&m_uvServer, bDelay);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CTCPServer::SetKeepAlive(BOOL bAlive, UINT nDelay)
{
	int nRet = uv_tcp_keepalive(&m_uvServer, bAlive, nDelay);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CTCPServer::Bind(LPCTSTR lpszListenIP, WORD wPort)
{
	SOCKADDR_IN BindAddr;
	int nRet = uv_ip4_addr(CStringA(lpszListenIP), wPort, &BindAddr);
	if (nRet)
		return FALSE;

	nRet = uv_tcp_bind(&m_uvServer, (SOCKADDR*)&BindAddr, 0);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CTCPServer::BindV6(LPCTSTR lpszListenIP, WORD wPort)
{
	sockaddr_in6 BindAddr;
	int nRet = uv_ip6_addr(CStringA(lpszListenIP), wPort, &BindAddr);

	if (nRet)
		return FALSE;

	nRet = uv_tcp_bind(&m_uvServer, (SOCKADDR*)&BindAddr,0);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CTCPServer::Listen(int nListen /* = 1024 */)
{
	int nRet = uv_listen((uv_stream_t*)&m_uvServer, nListen, AcceptConnection);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CTCPServer::Run(int nuvRunType /* = UV_RUN_DEFAULT */)
{
	int nRet = uv_run(m_uvpLoop, (uv_run_mode)nuvRunType);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CTCPServer::Start(LPCTSTR lpszListenIP, WORD wPort)
{
	Close();

	BOOL bSuccess = FALSE;
	do 
	{
		if (Init() == FALSE)
			break;
		if (Bind(lpszListenIP,wPort) == FALSE)
			break;
		if (Listen(SOMAXCONN) == FALSE)
			break;
		if (Run() == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CTCPServer::StartV6(LPCTSTR lpszListenIP, WORD wPort)
{
	Close();

	BOOL bSuccess = FALSE;
	do 
	{
		if (Init() == FALSE)
			break;
		if (BindV6(lpszListenIP,wPort) == FALSE)
			break;
		if (Listen(SOMAXCONN) == FALSE)
			break;
		if (Run() == FALSE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

int CTCPServer::Send(size_t ClientId, const char* data, size_t dwLen, BOOL bSend /*= TRUE*/)
{
	//服务器发送数据给客户端
	ClientConnMap::iterator itFind = ClientObjMap.find(ClientId);

	//找不到指定ID的客户端
	if (itFind == ClientObjMap.end())
		return -1;

	uv_buf_t& uvWriteBuf = itFind->second->m_uvWriteBuf;
	if (uvWriteBuf.len < dwLen)
	{
		uvWriteBuf.base = (char*)realloc(uvWriteBuf.base, dwLen);
		uvWriteBuf.len = dwLen;
	}

	memcpy(uvWriteBuf.base, data, dwLen);
	uv_buf_t Buf = uv_buf_init((char*)uvWriteBuf.base, dwLen);
	uv_write_t* uvWriteReq = new uv_write_t;					//在回调函数AfterSend中会释放
	int nRet = uv_write(uvWriteReq, (uv_stream_t*)itFind->second->m_uvhClient, &Buf, 1, AfterSend);

	return nRet;
}

void CTCPServer::AfterSend(uv_write_t* uvWriteReq, int nStatus)
{
	//不管是否发送成功，都需要释放内存
	if (uvWriteReq)
		delete uvWriteReq;

	if (nStatus < 0)
	{
		//运行到这里，说明发送出现问题
		exit(-1);
	}
}

void CTCPServer::SetConnectExtra(NewConnect cb)
{
	m_pNewConcb = cb;
}

void CTCPServer::SetRecvCB(size_t ClientId, ServerRecvcb cb)
{
	ClientConnMap::iterator itFind = ClientObjMap.find(ClientId);
	if (itFind != ClientObjMap.end())
		itFind->second->m_pRecvcb = cb;
}

void CTCPServer::onAllocBuffer(uv_handle_t *uvhClient, size_t suggested_size, uv_buf_t *ReadBuf) 
{ 
	//服务器分析空间函数 
	if (uvhClient->data == NULL)
		return; 
	ClientData* pClientData = (ClientData*)uvhClient->data; 
	*ReadBuf = pClientData->m_uvReadBuf; 
} 

void CTCPServer::AfterServerRecv(uv_stream_t *uvhClient, ssize_t nRead, const uv_buf_t* buf) 
{ 
	if (uvhClient->data == NULL)
		return; 

	ClientData* pClientData = (ClientData*)uvhClient->data;//服务器的recv带的是clientdata 
	if (nRead < 0)  
	{ 
		/* 错误 或 EOF结尾 */ 
		CTCPServer* pTcpSock = (CTCPServer *)pClientData->m_pTcpServer; 
		//string log_str; 
		//char tmp[512] = { 0 }; 
		if (nRead == UV_EOF)  
		{ 
			//log_str = "client("+itoa(client->client_id)+") is disconnected,close this client."; 

			//sprintf(tmp, "client(%d)连接断开，关闭此客户端", client->client_id); 
		} 
		else if (nRead == UV_ECONNRESET) 
		{ 
			//log_str = "client("+itoa(client->client_id)+") abortive disconnect."; 
			//sprintf(tmp, "客户端(%d)异常断开", client->client_id); 
		} 
		else  
		{ 
			//log_str = "client("+itoa(client->client_id)+")："+GetUVError(nread).c_str(); 
			//sprintf(tmp, "客户端(%d)：%s", client->client_id, GetUVError(nread).c_str()); 
		} 
		//dxLog_Base::WriteLineStr(log_str); 
		//连接断开，关闭客户端 
		pTcpSock->DeleteClient(pClientData->m_ClientId); 
		return; 
	} 
	else if (0 == nRead)  
	{ 
		/* 一切正常，只是没有读取任何数据 */ 
	} 
	else if (pClientData->m_pRecvcb)  
	{ 
		pClientData->m_pRecvcb(pClientData->m_ClientId, buf->base, nRead); 
	} 
} 

void CTCPServer::AfterClientClose(uv_handle_t* uvhClient)
{
	ClientData* pClientData = (ClientData*)uvhClient->data;
	delete pClientData;
}

void CTCPServer::AfterServerClose(uv_handle_t* uvhClient)
{
	ClientData* pClientData = (ClientData*)uvhClient->data;
	if (pClientData)
		delete pClientData;
}

void CTCPServer::AcceptConnection(uv_stream_t* uvServer, int nStatus)
{
	//服务器接收客户端连接
	if (uvServer->data == NULL)
		return;
	
	BOOL bSuccess = FALSE;
	ClientData* pClientData = NULL;
	do 
	{
		CTCPServer* pTCPSock = (CTCPServer*)uvServer->data;

		static size_t ClientId = 0;
		++ClientId;
		pClientData = new ClientData(ClientId);
		if (pClientData == NULL)
			break;

		pClientData->m_pTcpServer = pTCPSock;

		int nRet = uv_tcp_init(uvServer->loop, pClientData->m_uvhClient);
		if (nRet)
			break;

		nRet = uv_accept(uvServer, (uv_stream_t*)pClientData->m_uvhClient);
		if (nRet)
		{
			uv_close((uv_handle_t*)pClientData->m_uvhClient, NULL);
			break;
		}

		//加入到链接队列中
		pTCPSock->ClientObjMap.insert(std::make_pair(ClientId, pClientData));
		if (pTCPSock->m_pNewConcb)
			pTCPSock->m_pNewConcb(ClientId);

		nRet = uv_read_start((uv_stream_t*)pClientData->m_uvhClient, onAllocBuffer, AfterServerRecv);
		if (nRet)
		{
			uv_close((uv_handle_t*)pClientData->m_uvhClient, NULL);
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
	{
		if (pClientData != NULL)
		{
			delete pClientData;
			pClientData = NULL;
		}
	}
}

BOOL CTCPServer::DeleteClient(size_t ClientId)
{
	uv_mutex_lock(&m_uvhMutex);

	ClientConnMap::iterator itFind = ClientObjMap.find(ClientId);
	if (itFind == ClientObjMap.end())
	{
		uv_mutex_unlock(&m_uvhMutex);
		return FALSE;
	}

	if (uv_is_active((uv_handle_t*)itFind->second->m_uvhClient))
		uv_read_stop((uv_stream_t*)itFind->second->m_uvhClient);
	uv_close((uv_handle_t*)itFind->second->m_uvhClient, AfterClientClose);

	ClientObjMap.erase(itFind);
	uv_mutex_unlock(&m_uvhMutex);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//ClientData
////////////////////////////////////////////////////////////////////////////////
ClientData::ClientData(size_t ClientId)
	:m_ClientId(ClientId)
	,m_pRecvcb(NULL)
{
	m_uvhClient = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	m_uvhClient->data = this;

	m_uvReadBuf = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
	m_uvWriteBuf = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
}

ClientData::~ClientData()
{
	free(m_uvReadBuf.base);
	m_uvReadBuf.base = NULL;
	m_uvReadBuf.len = 0;

	free(m_uvWriteBuf.base);
	m_uvWriteBuf.base = NULL;
	m_uvWriteBuf.len = 0;

	free(m_uvhClient);
	m_uvhClient = NULL;
}