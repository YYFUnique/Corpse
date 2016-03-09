#include "StdAfx.h"
#include "WinSocket.h"

CMsgHead::CMsgHead(DWORD dwMsgId,LPCTSTR lpData)
	: m_dwMsgId(dwMsgId)
	, m_strData(lpData)
{
	m_pMsg = NULL;
}

CMsgHead::CMsgHead(PMSG_HEAD pMsg)
	: m_dwMsgId(pMsg->dwMsgId)
{
	m_strData = pMsg->szData;
	
	m_pMsg = NULL;
}

CMsgHead::CMsgHead(CMsgHead* pMsg)
	: m_dwMsgId(pMsg->GetMsgId())
	, m_strData(pMsg->GetMsgValue())
{
	m_pMsg = NULL;
}

CString CMsgHead::GetMsgValue()
{
	return m_strData;
}
DWORD CMsgHead::GetMsgId()
{
	return m_dwMsgId;
}

PMSG_HEAD CMsgHead::GetMsgHead()
{
	if (m_pMsg == NULL)
	{
		DWORD dwSize = GetMsgHeadLen();
		m_pMsg = (PMSG_HEAD)new BYTE[dwSize];

		m_pMsg->dwMsgId = m_dwMsgId;
		m_pMsg->dwDataLen = GetDataLen();
		
		lstrcpy(m_pMsg->szData,m_strData);
	}

	return m_pMsg;
}

DWORD CMsgHead::GetDataLen()
{
	return m_strData.GetLength() * sizeof(TCHAR);
}

DWORD CMsgHead::GetMsgHeadLen()
{
	return sizeof(MSG_HEAD) + GetDataLen();
}

BOOL CMsgHead::SetData(LPCTSTR lpData)
{
	m_strData = lpData;
	return TRUE;
}

CMsgHead::~CMsgHead()
{
	if (m_pMsg != NULL)
	{
		delete m_pMsg;
		m_pMsg = NULL;
	}
}

CWinSocket::CWinSocket(void)
{
	m_EventFunc.pOnClose = NULL;
	m_EventFunc.pOnReceive = NULL;
	m_EventFunc.pOnConnect = NULL;
	m_EventFunc.pOnError = NULL;
	m_EventFunc.pOnSend = NULL;
	m_EventFunc.pOnAccept = NULL;
}

CWinSocket::~CWinSocket(void)
{
	
}

ISocketListener::EnHandleResult CWinSocket::OnSend(CONNID dwConnID, const BYTE* pData, int iLength)
{
	if (m_EventFunc.pOnSend != NULL)
		if (m_EventFunc.pOnSend(dwConnID,pData,iLength) != ISocketListener::HR_OK)
			return ISocketListener::HR_IGNORE;

	//::PostOnSend(dwConnID,pData,iLength);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CWinSocket::OnReceive(CONNID dwConnID, const BYTE* pData, int iLength)
{
	if (m_EventFunc.pOnReceive != NULL)
		if (m_EventFunc.pOnReceive(dwConnID,pData,iLength) != ISocketListener::HR_OK)
			return ISocketListener::HR_IGNORE;

	//::PostOnReceive(dwConnID,pData,iLength);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CWinSocket::OnClose(CONNID dwConnID)
{
	if (m_EventFunc.pOnClose != NULL)
		if (m_EventFunc.pOnClose(dwConnID) != ISocketListener::HR_OK)
			return ISocketListener::HR_IGNORE;

	//::PostOnClose(dwConnID);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CWinSocket::OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	if (m_EventFunc.pOnError != NULL)
		if (m_EventFunc.pOnError(dwConnID,enOperation,iErrorCode) != ISocketListener::HR_OK)
			return ISocketListener::HR_IGNORE;

	//::PostOnError(dwConnID,enOperation,iErrorCode);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CWinSocket::OnConnect(CONNID dwConnID)
{
	if (m_EventFunc.pOnConnect != NULL)
		if (m_EventFunc.pOnConnect(dwConnID,m_strIPAddr,m_wPort) != ISocketListener::HR_OK)
			return ISocketListener::HR_IGNORE;

	ASSERT(m_strIPAddr.IsEmpty()==FALSE);

	//::PostOnConnect(dwConnID,m_strIPAddr,m_wPort);
	return ISocketListener::HR_OK;
}

ISocketListener::EnHandleResult CWinSocket::OnAccept(CONNID dwConnID, UINT_PTR soClient)
{
	if (m_EventFunc.pOnAccept != NULL)
		if (m_EventFunc.pOnAccept(dwConnID,soClient) != ISocketListener::HR_OK)
			return ISocketListener::HR_IGNORE;

	ASSERT(m_strIPAddr.IsEmpty()==FALSE);

	//::PostOnAccept(dwConnID, szAddress, usPort, bPass);
	return ISocketListener::HR_OK;
}

BOOL CWinSocket::Start(LPCTSTR pszRemoteAddress,USHORT usPortt)
{
	ASSERT(pszRemoteAddress || usPortt !=0 );
	m_strIPAddr = pszRemoteAddress;
	m_wPort = usPortt;

	return TRUE;
}

BOOL CWinSocket::RegEventFunc(PEVENT_FUNC EventFunc)
{
	ASSERT(EventFunc);
	m_EventFunc.pOnClose = EventFunc->pOnClose;
	m_EventFunc.pOnConnect = EventFunc->pOnConnect;
	m_EventFunc.pOnError = EventFunc->pOnError;
	m_EventFunc.pOnReceive = EventFunc->pOnReceive;
	m_EventFunc.pOnSend = EventFunc->pOnSend;
	m_EventFunc.pOnAccept = EventFunc->pOnAccept;

	return TRUE;
}

CClientSocket::CClientSocket()
	: m_TcpClient(this)
{

}

CClientSocket::~CClientSocket()
{
	Stop();
	Release();
}

BOOL CClientSocket::Start(LPCTSTR pszRemoteAddress, USHORT usPortt, BOOL bAsyncConnect /*= FALSE*/)
{
	CWinSocket::Start(pszRemoteAddress,usPortt);
	BOOL bRet = m_TcpClient.Start(pszRemoteAddress,usPortt,bAsyncConnect);
	
	return bRet;
}

BOOL CClientSocket::Stop()
{
	m_TcpClient.Stop();
	return TRUE;
}

BOOL CClientSocket::Send(CONNID ConnId,PMSG_HEAD pMsgHead,DWORD dwLen)
{
	return Send(ConnId,(PBYTE)pMsgHead,dwLen);
}

BOOL CClientSocket::Send(CONNID ConnId,const BYTE* pBuffer, int iLength) 
{
	return m_TcpClient.Send(ConnId,pBuffer,iLength);
}

BOOL CClientSocket::Send(CONNID ConnId,CString& strData)
{
	return Send(ConnId,(PBYTE)(LPCTSTR)strData,strData.GetLength()*sizeof(TCHAR));
}

BOOL CClientSocket::Send(CONNID ConnId,LPCTSTR lpData,int iLength) 
{
	return Send(ConnId,(PBYTE)lpData,iLength);
}

BOOL CClientSocket::Release()
{
	
	return TRUE;
}

EnServiceState CClientSocket::GetConnStat()
{
	return m_TcpClient.GetState();
}

CServerSocket::CServerSocket()
	: m_TcpServer(this)
{

}

BOOL CServerSocket::Start(LPCTSTR pszBindAddress, USHORT usPortt)
{
	CWinSocket::Start(pszBindAddress,usPortt);

	return m_TcpServer.Start(pszBindAddress,usPortt);
}

BOOL CServerSocket::Stop()
{
	m_TcpServer.Stop();
	return TRUE;
}

CServerSocket::~CServerSocket()
{
	Stop();
	Release();
}

BOOL CServerSocket::Send(CONNID ConnId,PMSG_HEAD pMsgHead,DWORD dwLen)
{
	return Send(ConnId,(PBYTE)pMsgHead,dwLen);
}

BOOL CServerSocket::Send(CONNID ConnId,CString& strData)
{
	return Send(ConnId,(PBYTE)(LPCTSTR)strData,strData.GetLength()*sizeof(TCHAR));
}

BOOL CServerSocket::Send(CONNID ConnId,LPCTSTR lpData,int iLength)
{
	return Send(ConnId,(PBYTE)lpData,iLength);
}

BOOL CServerSocket::Send(CONNID ConnId,const BYTE* pBuffer, int iLength)
{
	return m_TcpServer.Send(ConnId,pBuffer,iLength);
}

BOOL CServerSocket::Release()
{
	return TRUE;
}

EnServiceState CServerSocket::GetConnStat()
{
	return m_TcpServer.GetState();
}

BOOL CServerSocket::GetClientAddress(CONNID dwConnID, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	return m_TcpServer.GetClientAddress(dwConnID,lpszAddress,iAddressLen,usPort);
}