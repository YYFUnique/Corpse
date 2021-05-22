#pragma once

namespace NetCore
{
	class NET_API HTTPServerResponse : public HTTPResponse
	{
	public:
		HTTPServerResponse();
		~HTTPServerResponse();
	public:
		/// Sends a 100 Continue response to the client.
		virtual void SendContinue() = 0;
		/*virtual std::ostream& send() = 0; */
		virtual void SendFile(LPCTSTR lpszPath, LPCTSTR lpszMediaType) = 0;
		virtual void SendBuffer(LPVOID pBuffer, size_t Len) = 0;
		/// Sets the status code, which must be one of
		/// HTTP_MOVED_PERMANENTLY (301), HTTP_FOUND (302),
		/// or HTTP_SEE_OTHER (303),
		/// and sets the "Location" header field
		/// to the given URI, which according to
		/// the HTTP specification, must be absolute.
		///
		/// Must not be called after send() has been called.
		virtual void Redirect(LPCTSTR lpszURI, HTTPStatus Status = HTTP_FOUND) = 0;
		virtual void RequireAuthentication(LPCTSTR lpszRealm) = 0;
		virtual BOOL Sent() const = 0;
	};
}