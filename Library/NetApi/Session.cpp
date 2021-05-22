#include "StdAfx.h"
#include "Session.h"

CSessionData::CSessionData(size_t nClientId)
{
	m_uReadOffset = 0;
	m_uvReadBuf = NULL;
	m_uIndex = nClientId;
	m_pServer = NULL;
	m_pHandler = NULL;

	m_pStream = (uv_stream_t*)malloc(sizeof(uv_tcp_t));
	m_pStream->data = this;
}

CSessionData::~CSessionData()
{
	m_uIndex = 0;
	free(m_pStream);
	m_pStream = NULL;

	if (m_uvReadBuf != NULL)
	{
		free(m_uvReadBuf);
		m_uvReadBuf = NULL;
	}
	m_uReadOffset = 0;
}

uv_stream_t* CSessionData::GetStream() const
{
	return m_pStream;
}

void CSessionData::SetServer(INetServer* pServer)
{
	m_pServer = pServer;
}

INetServer* CSessionData::GetServer() const
{
	return m_pServer;
}

void CSessionData::SetHandler(IHandler* pHandler)
{
	m_pHandler = pHandler;
}

IHandler* CSessionData::GetHandler() const
{
	return m_pHandler;
}

size_t CSessionData::GetClientId() const
{
	return m_uIndex;
}

void CSessionData::Close()
{
	if (uv_is_active((uv_handle_t*)&m_pStream))
		uv_read_stop((uv_stream_t*)&m_pStream);
	uv_close((uv_handle_t*)m_pStream, OnClose);
}

BOOL CSessionData::Send(LPCSTR lpszData, size_t nLen)
{
	write_req_t* pReq = (write_req_t*)malloc(sizeof(write_req_t));
	ASSERT(pReq);

	pReq->buf = uv_buf_init((const_cast<char*>(lpszData)), nLen);
	if (uv_write(&pReq->req, m_pStream, &pReq->buf, 1, AfterWrite))
	{
		LOG(_T("uv_write failed\n"));
		return FALSE;
	}
	return TRUE;
}

void CSessionData::OnMessage(LPCSTR lpszData, int nSize)
{
	if (m_uvReadBuf == NULL)
		m_uvReadBuf = (char*)malloc(nSize);
	else
		m_uvReadBuf = (char*)realloc(m_uvReadBuf, m_uReadOffset+nSize);

	memcpy((char*)m_uvReadBuf+m_uReadOffset, lpszData, nSize);
	m_uReadOffset += nSize;
}

void CSessionData::OnRecv()
{
	if (m_pHandler)
		m_pHandler->Recv(this, m_uvReadBuf, m_uReadOffset);

	m_uReadOffset = 0;
	if (m_uvReadBuf != NULL)
	{
		free(m_uvReadBuf);
		m_uvReadBuf = NULL;
	}
}

void CSessionData::AfterWrite(uv_write_t* pReq, int nStatus)
{
	CSessionData* pClientData = (CSessionData*)pReq->handle->data;
	if (nStatus == NO_ERROR)
	{
		free(pReq);
		return ;
	}

	LOG(_T("uv_write error: %s - %s\n"), CString(uv_err_name(nStatus)), CString(uv_strerror(nStatus)));

	if (uv_is_closing((uv_handle_t*)pReq->handle) == FALSE)
		pClientData->Close();

	free(pReq);
}

void CSessionData::OnClose(uv_handle_t *uvhClient)
{
	CSessionData* pClientData = (CSessionData*)uvhClient->data;
	delete pClientData;
}