#pragma once

class ISession;
interface IHandler
{
public:
	virtual void Init() = 0;
	virtual void OnClose() = 0;
	virtual void Recv(ISession* pSession, LPCSTR lpszData, int nLen) = 0;
};