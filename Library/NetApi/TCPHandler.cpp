#include "StdAfx.h"
#include "TCPHandler.h"

CTCPHandler::CTCPHandler()
{

}

CTCPHandler::~CTCPHandler()
{

}

void CTCPHandler::Init()
{

}

void CTCPHandler::Recv(ISession* pSession, LPCSTR lpszData, int nLen)
{
	/*pSession->Send(lpszData, nLen);*/
}

void CTCPHandler::OnClose()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////
/////
//////////////////////////////////////////////////////////////////////////

CAsynTCPHandler::CAsynTCPHandler()
{

}

CAsynTCPHandler::~CAsynTCPHandler()
{

}

void CAsynTCPHandler::Init()
{

}

void CAsynTCPHandler::OnClose()
{
	delete this;
}

void CAsynTCPHandler::Recv(ISession* pSession, int nClientId, LPCSTR lpszData, int nLen)
{

}
