#pragma once

interface ITCPHandler
{
public:
	virtual void RecvRoute(int nClientId, LPCSTR lpszData, int nLen) = 0;
	virtual void OnClose() = 0;
};