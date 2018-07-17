#include "StdAfx.h"
#include "TCPHandler.h"
#include "TCPServer.h"
extern CTCPServer g_TCPSrv; 

CTCPHandler::CTCPHandler()
{
	m_pADELLock = NULL;
}

CTCPHandler::~CTCPHandler()
{
	if (m_pADELLock != NULL)
	{
		m_pADELLock->Release();
		m_pADELLock = NULL;
	}
}

void CTCPHandler::Init()
{
	if (m_pADELLock == NULL)
		m_pADELLock = new CADELLock();
}

void CTCPHandler::OnClose()
{
	delete this;
}

void CTCPHandler::RecvRoute(int nClientId, LPCSTR lpszData, int nLen)
{
	if (m_pADELLock == NULL)
		return;
	g_TCPSrv.Send(nClientId, lpszData, nLen);
	LPCSTR  lpszDatax = "abcdefghijklmn";
	g_TCPSrv.Send(nClientId, lpszDatax, strlen(lpszDatax));
}