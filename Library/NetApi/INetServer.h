#pragma once

typedef enum tagNETSERVER_TYPE
{
	NETSERVER_TYPE_BASE	= 0,
	NETSERVER_TYPE_TCP	= 1,
	NETSERVER_TYPE_UDP	= 2,
}NETSERVER_TYPE;

typedef uv_run_mode UV_RUN_MODE;

typedef BOOL (* FN_Connect)(size_t ClientId);

class NETSERVER_API INetServer
{
public:
	virtual BOOL Init(LPCTSTR lpszServerAddr, WORD wPort, UINT nMax) = 0;
	virtual void Start(UV_RUN_MODE uvMode = UV_RUN_DEFAULT) = 0;
	virtual void Shutdown() = 0;
	virtual LPVOID GetInterface(NETSERVER_TYPE ServerType);
	virtual const uv_loop_t* GetLoop() const;
	void SetConnectExtra(FN_Connect cb);
	virtual void SetHandlerCB(size_t nClientId, IHandler* pHandler) = 0;
public:
	void Release();
	static INetServer* CreateServer(NETSERVER_TYPE ServerType, uv_loop_t* loop = uv_default_loop());
protected:
	INetServer();
	virtual ~INetServer();
protected:
	FN_Connect		m_pfnConcb;
	uv_loop_t*		m_pLoop;
};