#pragma once

typedef struct tagwrite_req_t
{
	uv_write_t req;
	uv_buf_t buf;
} write_req_t;

class ISession 
{
public:
	ISession(){}
	virtual ~ISession(){}
	virtual void OnMessage(LPCSTR lpszData, int nSize) = 0;
	virtual int Send(LPCSTR lpszData, size_t nLen) = 0;
	virtual void Close() = 0;
};

class CSessionData : public ISession
{
public:
	CSessionData(size_t nClientId);
	virtual ~CSessionData();
public:
	uv_stream_t* GetStream() const;
	void SetServer(INetServer* pServer);
	INetServer* GetServer() const;
	void SetHandler(IHandler* pHandler);
	IHandler* GetHandler() const;
	size_t GetClientId() const;
public:
	virtual void Close();
	void OnRecv();
	virtual BOOL Send(LPCSTR lpszData, size_t nLen);
	virtual void OnMessage(LPCSTR lpszData, int nSize);	
protected:
	static void AfterWrite(uv_write_t* pReq, int nStatus);
	static void OnClose(uv_handle_t* pPeer);

protected:
	size_t				m_uIndex;
	IHandler*			m_pHandler;
	INetServer*		m_pServer;
	uv_stream_t*	m_pStream;
	size_t				m_uReadOffset;
	char*				m_uvReadBuf;		//接收数据的buf
};