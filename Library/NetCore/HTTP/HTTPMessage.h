#pragma once

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
	#define	EMPTY														_T("")
	#define	UNKNOWN_CONTENT_LENGTH			-1
	class NET_API HTTPMessage : public MessageHeader
	{
	public:
		void SetVersion(LPCTSTR lpszVersion);
		LPCTSTR GetVersion() const;

		void SetContentLength(DWORD dwLen);
		DWORD GetContentLength() const;

		BOOL HasContentLength() const;

		void SetTransferEncoding(LPCTSTR lpszTransferEncoding);
		CString GetTransferEncoding() const;

		void SetChunkedTransferEncoding(BOOL bFlag);
		BOOL GetChunkedTransferEncoding() const;

		void SetContentType(LPCTSTR lpszMediaType);
		void SetContentType(const MediaType& MimeType);

		CString GetContentType() const;

		void SetKeepAlive(BOOL bKeepAlive);
		BOOL IsKeepAlive() const;

	protected:
		HTTPMessage();
		HTTPMessage(LPCTSTR lpszHttpVersion);
		virtual ~HTTPMessage();
	private:
		HTTPMessage(const HTTPMessage&);
		HTTPMessage& operator=(const HTTPMessage&);
	private:
		CString m_strVersion;
	};
}