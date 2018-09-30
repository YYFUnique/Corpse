#include "StdAfx.h"
#include "HTTPMessage.h"

namespace NetCore
{
#define	HTTP_1_0													_T("HTTP/1.0")
#define	HTTP_1_1													_T("HTTP/1.1")
#define	IDENTITY_TRANSFER_ENCODING			_T("identity")
#define	CHUNKED_TRANSFER_ENCODING			_T("chunked")
#define	CONTENT_LENGTH									_T("Content-Length")
#define	CONTENT_TYPE										_T("Content-Type")
#define	TRANSFER_ENCODING							_T("Transfer-Encoding")
#define	CONNECTION											_T("Connection")
#define	CONNECTION_KEEP_ALIVE						_T("Keep-Alive")
#define	CONNECTION_CLOSE								_T("Close")

#define	UNKNOWN_CONTENT_LENGTH			-1

	HTTPMessage::HTTPMessage()
		: m_strVersion(HTTP_1_0)
	{

	}

	HTTPMessage::HTTPMessage(LPCTSTR lpszHttpVersion)
		: m_strVersion(lpszHttpVersion)
	{

	}

	HTTPMessage::~HTTPMessage()
	{

	}

	void HTTPMessage::SetVersion(LPCTSTR lpszVersion)
	{
		m_strVersion = lpszVersion;
	}

	LPCTSTR HTTPMessage::GetVersion() const
	{
		return m_strVersion;
	}

	void HTTPMessage::SetContentLength(DWORD dwLen)
	{
		TCHAR szContenLen[20];
		_stprintf_s(szContenLen, _countof(szContenLen), _T("%u"), dwLen);
		if (dwLen != UNKNOWN_CONTENT_LENGTH)
			Set(CONTENT_TYPE, szContenLen);
		else
			Erase(CONTENT_TYPE);
	}

	DWORD HTTPMessage::GetContentLength() const
	{
		CString strContentLen = Get(CONTENT_LENGTH, _T(""));
		if (strContentLen.IsEmpty() == FALSE)
			return _ttoi(strContentLen);
		else
			return UNKNOWN_CONTENT_LENGTH;
	}

	void HTTPMessage::SetTransferEncoding(LPCTSTR lpszTransferEncoding)
	{
		if (_tcsicmp(lpszTransferEncoding, IDENTITY_TRANSFER_ENCODING) == 0)
			Erase(TRANSFER_ENCODING);
		else
			Set(TRANSFER_ENCODING, lpszTransferEncoding);
	}

	CString HTTPMessage::GetTransferEncoding() const
	{
		return Get(TRANSFER_ENCODING, IDENTITY_TRANSFER_ENCODING);
	}

	void HTTPMessage::SetChunkedTransferEncoding(BOOL bFlag)
	{
		if (bFlag)
			SetTransferEncoding(CHUNKED_TRANSFER_ENCODING);
		else
			SetTransferEncoding(IDENTITY_TRANSFER_ENCODING);
	}

	BOOL HTTPMessage::GetChunkedTransferEncoding() const
	{
		return _tcsicmp(GetTransferEncoding(), CHUNKED_TRANSFER_ENCODING) == 0;
	}

	void HTTPMessage::SetContentType(LPCTSTR lpszMediaType)
	{
		if (_tcslen(lpszMediaType) == 0)
			Erase(CONTENT_TYPE);
		else
			Set(CONTENT_TYPE, lpszMediaType);
	}

	void HTTPMessage::SetContentType(const MediaType& MimeType)
	{
		SetContentType(MimeType.ToString());
	}

	CString HTTPMessage::GetContentType() const
	{
		return Get(CONTENT_TYPE, _T(""));
	}

	void HTTPMessage::SetKeepAlive(BOOL bKeepAlive)
	{
		if (bKeepAlive)
			Set(CONNECTION, CONNECTION_KEEP_ALIVE);
		else
			Set(CONNECTION, CONNECTION_CLOSE);
	}

	BOOL HTTPMessage::IsKeepAlive() const
	{
		CString strConnection = Get(CONNECTION, _T(""));
		if (strConnection.IsEmpty() == FALSE)
			return _tcsicmp(strConnection, CONNECTION_CLOSE) != 0;
		else
			return _tcsicmp(GetVersion(), HTTP_1_1) == 0;
	}
}