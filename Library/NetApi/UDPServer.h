#pragma once

class CUDPServer : public INetServer
{
public:
	CUDPServer();
protected:
	virtual ~CUDPServer();
public:
	virtual BOOL Init(LPCTSTR lpszServerAddr, WORD wPort, UINT nMax);
	virtual void Start(UV_RUN_MODE uvMode);
	virtual void Shutdown();
	virtual LPVOID GetInterface(NETSERVER_TYPE ServerType);
	virtual void SetHandlerCB(size_t nClientId, IHandler* pHandler);
protected:
	uv_udp_t m_udp_server;
};