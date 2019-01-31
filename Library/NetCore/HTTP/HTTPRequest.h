#pragma once

namespace NetCore
{
	#define HTTP_GET								_T("GET")
	#define HTTP_HEAD							_T("HEAD")
	#define HTTP_PUT								_T("PUT")
	#define HTTP_POST							_T("POST")
	#define HTTP_OPTIONS						_T("OPTIONS")
	#define HTTP_DELETE							_T("DELETE")
	#define HTT_PTRACE							_T("TRACE")
	#define HTTP_CONNECT					_T("CONNECT")
	#define HTTP_PATCH							_T("PATCH")
	#define HOST										_T("Host")
	#define COOKIE									_T("Cookie")
	#define AUTHORIZATION					_T("Authorization")
	#define PROXY_AUTHORIZATION		_T("Proxy-Authorization")
	#define UPGRADE								_T("Upgrade")
	#define EXPECT									_T("Expect")

	/// This class encapsulates an HTTP request
	/// message.
	///
	/// In addition to the properties common to
	/// all HTTP messages, a HTTP request has
	/// a method (e.g. GET, HEAD, POST, etc.) and
	/// a request URI.
	class NET_API HTTPRequest : public HTTPMessage
	{
	public:
		/// Creates a GET / HTTP/1.0 HTTP request.
		HTTPRequest();

		/// Creates a GET / HTTP/1.x request with
		/// the given version (HTTP/1.0 or HTTP/1.1).
		HTTPRequest(LPCTSTR lpszVersion);

		/// Creates a HTTP/1.0 request with the given method and URI.
		HTTPRequest(LPCTSTR lpszMethod, LPCTSTR lpszUri);

		/// Creates a HTTP request with the given method, URI and version.
		HTTPRequest(LPCTSTR strMethod, LPCTSTR lpszUri, LPCTSTR lpszVersion);
		
		virtual ~HTTPRequest();

	public:
		void SetMethod(LPCTSTR lpszMethod);
		CString GetMethod() const;

		void SetURI(LPCTSTR lpszURI);
		CString GetURI() const;

		void SetHost(LPCTSTR lpszHost);
		void SetHost(LPCTSTR lpszHost, WORD wPort);
		CString GetHost() const;

		void SetCookies(const NameValueCollection& Cookies);
		void GetCookies(NameValueCollection& Cookies) const;

		BOOL HasCredentials() const;
		void GetCredentials(CString& strScheme, CString& strAuthInfo) const;
		void SetCredentials(LPCTSTR lpszScheme, LPCTSTR lpszAuthInfo);

		/// Returns true if the request contains an
		/// "Expect: 100-continue" header.
		BOOL GetExpectContinue() const;
		/// Adds a "Expect: 100-continue" header to the request if
		/// expectContinue is true, otherwise removes the Expect header.
		void SetExpectContinue(BOOL bExpectContinue);

		BOOL HasProxyCredentials() const;
		void SetProxyCredentials(LPCTSTR lpszScheme, LPCTSTR lpszAuthInfo);
		void GetProxyCredentials(CString& strScheme, CString& strAuthInfo) const;

	protected:
		HTTPRequest(const HTTPRequest&);
		HTTPRequest& operator= (const HTTPRequest&);
	protected:
		void GetCredentials(LPCTSTR lpszHeader, CString& strScheme, CString& strAuthInfo) const;
		void SetCredentials(LPCTSTR lpszHeader, LPCTSTR lpszScheme, LPCTSTR lpszAuthInfo);
	protected:
		CString		m_strMethod;
		CString		m_strURI;
	};
}