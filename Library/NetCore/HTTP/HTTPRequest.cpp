#include "StdAfx.h"
#include "HTTPRequest.h"

namespace NetCore
{
	HTTPRequest::HTTPRequest()
		: m_strMethod(HTTP_GET)
		, m_strURI(_T("/"))
	{

	}

	HTTPRequest::HTTPRequest(LPCTSTR lpszVersion)
		: HTTPMessage(lpszVersion)
		, m_strMethod(HTTP_GET)
		, m_strURI(_T("/"))
	{

	}

	HTTPRequest::HTTPRequest(LPCTSTR lpszMethod, LPCTSTR lpszUri)
		: m_strMethod(lpszMethod)
		, m_strURI(lpszUri)
	{

	}

	HTTPRequest::HTTPRequest(LPCTSTR lpszMethod, LPCTSTR lpszUri, LPCTSTR lpszVersion)
		: HTTPMessage(lpszVersion)
		, m_strMethod(lpszMethod)
		, m_strURI(lpszUri)
	{

	}

	HTTPRequest::~HTTPRequest()
	{

	}

	void HTTPRequest::SetMethod(LPCTSTR lpszMethod)
	{
		m_strMethod = lpszMethod;
	}

	CString HTTPRequest::GetMethod() const
	{
		return m_strMethod;
	}

	void HTTPRequest::SetURI(LPCTSTR lpszURI)
	{
		m_strURI = lpszURI;
	}

	CString HTTPRequest::GetURI() const
	{
		return m_strURI;
	}

	void HTTPRequest::SetHost(LPCTSTR lpszHost)
	{
		Set(HOST, lpszHost);
	}

	void HTTPRequest::SetHost(LPCTSTR lpszHost, WORD wPort)
	{
		CString strHost = lpszHost;
		if (strHost.Find(_T(':')) != -1)
		{
			// IPv6 address
		}

		if (wPort != 80 && wPort != 443)
		{

		}

		SetHost(strHost);
	}

	CString HTTPRequest::GetHost() const
	{
		return Get(HOST);
	}

	void HTTPRequest::SetCookies(const NameValueCollection& Cookies)
	{
		CString strCookie;
		for (ConstIterator it = Cookies.Begin(); it != Cookies.End(); ++it)
		{
			if (it != Cookies.Begin())
				strCookie.Append(_T("; "));
			strCookie.Append(it->first);
			strCookie.Append(_T("="));
			strCookie.Append(it->second);
		}
		Add(COOKIE, strCookie);
	}

	void HTTPRequest::GetCookies(NameValueCollection& Cookies) const
	{
		ConstIterator it = Find(COOKIE);
		//while(it != End() &&)
	}

	BOOL HTTPRequest::HasCredentials() const
	{
		return Has(AUTHORIZATION);
	}

	void HTTPRequest::GetCredentials(CString& strScheme, CString& strAuthInfo) const
	{
		GetCredentials(AUTHORIZATION, strScheme, strAuthInfo);
	}

	void HTTPRequest::SetCredentials(LPCTSTR lpszScheme, LPCTSTR lpszAuthInfo)
	{
		SetCredentials(AUTHORIZATION, lpszScheme, lpszAuthInfo);
	}

	BOOL HTTPRequest::HasProxyCredentials() const
	{
		return Has(PROXY_AUTHORIZATION);
	}

	void HTTPRequest::GetProxyCredentials(CString& strScheme, CString& strAuthInfo) const
	{
		GetCredentials(PROXY_AUTHORIZATION, strScheme, strAuthInfo);
	}

	void HTTPRequest::SetProxyCredentials(LPCTSTR lpszScheme, LPCTSTR lpszAuthInfo)
	{
		SetCredentials(PROXY_AUTHORIZATION, lpszScheme, lpszAuthInfo);
	}

	void HTTPRequest::SetCredentials(LPCTSTR lpszHeader, LPCTSTR lpszScheme, LPCTSTR lpszAuthInfo)
	{
		CString strAuth(lpszScheme);
		strAuth.AppendChar(_T(' '));
		strAuth.Append(lpszAuthInfo);
		Set(lpszHeader, strAuth);
	}

	void HTTPRequest::GetCredentials(LPCTSTR lpszHeader, CString& strScheme, CString& strAuthInfo) const
	{
		strScheme.Empty();
		strAuthInfo.Empty();
		if (Has(lpszHeader))
		{

		}
	}

	BOOL HTTPRequest::GetExpectContinue() const
	{
		CString strExpect = Get(EXPECT, EMPTY);
		return strExpect.IsEmpty() == FALSE && strExpect.CompareNoCase(_T("100-continue")) == 0;
	}

	void HTTPRequest::SetExpectContinue(BOOL bExpectContinue)
	{
		if (bExpectContinue)
			Set(EXPECT, _T("100-continue"));
		else
			Erase(EXPECT);
	}

	HTTPRequest::HTTPRequest(const HTTPRequest& Request)
	{
		if (&Request != this)
		{
			m_strMethod = Request.GetMethod();
			m_strURI = Request.GetURI();
		}
	}

	HTTPRequest& HTTPRequest::operator= (const HTTPRequest& Request)
	{
		if (&Request != this)
		{
			m_strMethod = Request.GetMethod();
			m_strURI = Request.GetURI();
		}
		return *this;
	}
}