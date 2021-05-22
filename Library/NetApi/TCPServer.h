#pragma once

class CSessionData;
typedef std::map<size_t, CSessionData*> ClientConnMap;

#define NONE_ALLOC_BUFFER				-1		// 未分配服务器接收数据缓冲区大小
#define DEFAULT_ALLOC_BUFFER			256	// 服务器接收数据默认分配空间大小

class NETSERVER_API CTCPServer : public INetServer
{
public:
	CTCPServer();
protected:
	virtual ~CTCPServer();
public:
	virtual BOOL Init(LPCTSTR lpszServerAddr, WORD wPort, UINT nMax);
	virtual void Start(UV_RUN_MODE uvMode = UV_RUN_DEFAULT);
	virtual void Shutdown();
	virtual LPVOID GetInterface(NETSERVER_TYPE ServerType);
	virtual void SetHandlerCB(size_t nClientId, IHandler* pHandler);
public:
	void SetAllocByte(DWORD dwAllocByte);
	DWORD GetAllocByte() const;
	int Send(size_t ClientId, LPCSTR lpszData, size_t nLen, BOOL bSend = TRUE);
protected:
	void DeleteClient(CSessionData* pClientData, size_t nClientId);
protected:
	static size_t GetAvailaClientID();
	static void OnConnect(uv_stream_t* uvStream, int nStatus);
	static void AfterServerRecv(uv_stream_t* pStream, ssize_t nRead, const uv_buf_t* buf);
	static void OnAllocBuffer(uv_handle_t *uvhClient, size_t suggested_size, uv_buf_t *ReadBuf);
protected:
	DWORD				m_dwAllocByte;
	uv_tcp_t				m_tcp_server;
	ClientConnMap ClientObjMap;			// 子客户端集合
};