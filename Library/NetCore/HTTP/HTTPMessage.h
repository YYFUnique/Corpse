#pragma once

namespace NetCore
{
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