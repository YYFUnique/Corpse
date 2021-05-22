#pragma once

class NETSERVER_API CTCPHandler : public IHandler
{
public:
	CTCPHandler();
protected:
	virtual ~CTCPHandler();
public:
	virtual void Init();
	virtual void OnClose();
	virtual void Recv(ISession* pSession, LPCSTR lpszData, int nLen);
};

class NETSERVER_API CAsynTCPHandler : public IHandler
{
public:
	CAsynTCPHandler();
protected:
	virtual ~CAsynTCPHandler();
public:
	virtual void Init();
	virtual void OnClose();
	virtual void Recv(ISession* pSession, int nClientId, LPCSTR lpszData, int nLen);
};