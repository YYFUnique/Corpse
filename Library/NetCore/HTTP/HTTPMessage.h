#pragma once

namespace NetCore
{
	class NET_API HTTPMessage
	{
	public:
		void SetVersion(LPCTSTR lpszVersion);
		LPCTSTR GetVersion() const;

		void SetContentLength(DWORD dwLen);
		DWORD GetContentLength() const;

		BOOL HasContentLength() const;

		void SetTransferEncoding(LPCTSTR lpszTransferEncoding);
		LPCTSTR GetTransferEncoding() const;

		void SetChunkedTransferEncoding(BOOL bFlag);
		BOOL GetChunkedTransferEncoding() const;

		void SetContentType(LPCTSTR lpszMediaType);
		void SetContentType(const MediaType& MimeType);

		LPCTSTR GetContentType() const;

		void SetKeepAlive(BOOL bKeepAlive);
		BOOL IsKeepAlive() const;

	};
}