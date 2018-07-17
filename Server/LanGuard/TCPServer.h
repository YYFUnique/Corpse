#pragma once

#include "libuv/uv.h"
#include <map>
#include <stdio.h>
#include <cstdlib>
#include "ITCPHandler.h"

typedef void (* FN_Connect)(size_t ClientId);
//typedef void (* ServerRecvcb)(size_t ClientId,const char* Buf, int nBufSize);

class ClientData;
typedef std::map<size_t, ClientData*> ClientConnMap;

class CTCPServer
{
public:
	CTCPServer(uv_loop_t* loop = uv_default_loop());
	virtual ~CTCPServer();

public:
	BOOL Start(LPCTSTR lpszListenIP, WORD wPort);

	BOOL StartV6(LPCTSTR lpszListenIP, WORD wPort);

	/************************************************************************/
	// 函数名称：Stop
	// 函数功能：退出由uv_run阻塞的操作
	// 函数参数：无
	// 返  回 值：无
	/************************************************************************/
	void Stop();

	/************************************************************************/
	// 函数名称：Close
	// 函数功能：清理客户端集合，释放内存
	// 函数参数：无
	// 返  回 值：无
	/************************************************************************/
	void Close();

	/************************************************************************/
	// 函数名称：CloseServer
	// 函数功能：主动关闭服务器套接字，不再接收新的连接
	// 函数参数：无
	// 返  回 值：无
	/************************************************************************/
	void CloseServer();

	BOOL IsServerClosed();

	/************************************************************************/
	// 函数名称：AutoClose
	// 函数功能：服务器主动关闭套接字后，不再回复已建立套接字的客户端
	// 函数参数：无
	// 返  回 值：无
	/************************************************************************/
	void SetAutoClose(BOOL bAutoClose);

	BOOL IsAutoClose();

	BOOL SetNoDelay(BOOL bDelay = FALSE);
	BOOL SetKeepAlive(BOOL bAlive, UINT nDelay);

	/************************************************************************/
	// 函数名称：Send
	// 函数功能：向客户端发送数据
	// 函数参数：ClientId 客户端连接标识
	//					 data		需要发送的数据
	//					 dwLen		发送数据长度
	// 返  回 值：发送长度
	/************************************************************************/
	int Send(size_t ClientId, const char* data, size_t dwLen, BOOL bSend = TRUE);

	/************************************************************************/
	// 函数名称：SetConnectExtra
	// 函数功能：客户端连接时回调函数
	// 函数参数：cb 回调函数指针
	// 返  回 值：无
	/************************************************************************/
	void SetConnectExtra(FN_Connect cb);

	/************************************************************************/
	// 函数名称：SetTCPHandlerCB
	// 函数功能：客户端处理过程
	// 函数参数：ClientId 客户端连接标识
	//					pTCPHandler 处理过程对象指针	
	// 返  回 值：无
	/************************************************************************/
	void SetTCPHandlerCB(size_t ClientId, ITCPHandler* pTCPHandler);

protected:
	size_t GetAvailaClientID() const;			// 获取可用的client id  
	BOOL DeleteClient(size_t ClientId);	// 删除链表中的客户端  
	void CloseClient(size_t nClientId);		// 删除链表中的客户端，如果客户端集合为空，关闭服务器

	static void AfterClientClose(uv_handle_t* uvhClient);
	static void AfterServerClose(uv_handle_t* uvhClient);
	static void AfterSend(uv_write_t* uvWriteReq, int nStatus);
	static void AcceptConnection(uv_stream_t* uvServer, int nStatus);
	static void AfterServerRecv(uv_stream_t *uvhClient, ssize_t nRead, const uv_buf_t* buf);
	static void OnAllocBuffer(uv_handle_t *uvhClient, size_t suggested_size, uv_buf_t *ReadBuf);
private:
	BOOL Init();
	BOOL Run(int nuvRunType = UV_RUN_DEFAULT);
	BOOL Bind(LPCTSTR lpszListenIP, WORD wPort);
	BOOL BindV6(LPCTSTR lpszListenIP, WORD wPort);
	BOOL Listen(int nListen = 1024);

private:
	BOOL				m_bServerClosed;		// 服务器主动关闭
	BOOL				m_bAutoClosed;			//	服务器关闭后关闭客户端，自动退出
	BOOL				m_bInit;						// 是否经过初始化
	uv_tcp_t			m_uvServer;					//	服务器链接
	uv_loop_t*		m_uvpLoop;				//	
	uv_mutex_t		m_uvhMutex;				//	客户端集合互斥量
	FN_Connect		m_pfnConcb;					// 新建连接
	ClientConnMap ClientObjMap;			// 子客户端集合
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