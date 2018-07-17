#pragma once

#include "libuv/uv.h"
#include <map>
#include <stdio.h>
#include <cstdlib>
#include "ITCPHandler.h"

typedef BOOL (* NewConnect)(size_t ClientId);
//typedef void (* ServerRecvcb)(size_t ClientId,const char* Buf, int nBufSize);

class ClientData;
typedef std::map<size_t, ClientData*> ClientConnMap;

class CTCPServer
{
public:
	CTCPServer(uv_loop_t* loop = uv_default_loop());
	virtual ~CTCPServer();

public:
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	BOOL Start(LPCTSTR lpszListenIP, WORD wPort);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	BOOL StartV6(LPCTSTR lpszListenIP, WORD wPort);

	void Stop();
	void Close();
	void CloseServer();

	BOOL SetNoDelay(BOOL bDelay = FALSE);
	BOOL SetKeepAlive(BOOL bAlive, UINT nDelay);

	int Send(size_t ClientId, const char* data, size_t dwLen, BOOL bSend = TRUE);

	void SetConnectExtra(NewConnect cb);
	void SetTCPHandlerCB(size_t ClientId, ITCPHandler* pTCPHandler);

protected:
	size_t GetAvailaClientID() const;				// 获取可用的client id  
	BOOL DeleteClient(size_t ClientId);	// 删除链表中的客户端  
	
	static void AfterClientClose(uv_handle_t* uvhClient);
	static void AfterServerClose(uv_handle_t* uvhClient);
	static void AfterSend(uv_write_t* uvWriteReq, int nStatus);
	static void AcceptConnection(uv_stream_t* uvServer, int nStatus);
	static void AfterServerRecv(uv_stream_t *uvhClient, ssize_t nRead, const uv_buf_t* buf);
	static void onAllocBuffer(uv_handle_t *uvhClient, size_t suggested_size, uv_buf_t *ReadBuf);
private:
	BOOL Init();
	BOOL Run(int nuvRunType = UV_RUN_DEFAULT);
	BOOL Bind(LPCTSTR lpszListenIP, WORD wPort);
	BOOL BindV6(LPCTSTR lpszListenIP, WORD wPort);
	BOOL Listen(int nListen = 1024);

private:
	BOOL			m_bClose;
	BOOL			m_bInit;
	uv_tcp_t		m_uvServer;		//服务器链接
	uv_loop_t*	m_uvpLoop;	//
	uv_mutex_t  m_uvhMutex;
	NewConnect m_pNewConcb;
	ClientConnMap ClientObjMap;//子客户端链接 
};

class ClientData
{
public:
	ClientData(size_t ClientId);
	virtual ~ClientData();

public:
	size_t			m_ClientId;			//客户端id，唯一
	uv_tcp_t*		m_uvhClient;		//客户端句柄
	uv_buf_t		m_uvReadBuf;		//接收数据的buf
	uv_buf_t		m_uvWriteBuf;		//写数据的buf
	CTCPServer* m_pTcpServer;	//服务器句柄
	//ServerRecvcb m_pRecvcb;		//接收数据回调给用户的函数
	ITCPHandler*	m_pHandler;
};