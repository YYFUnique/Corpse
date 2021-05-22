#include "StdAfx.h"
#include "HttpResponse.h"

namespace NetCore
{
	HTTPResponse::HTTPResponse()
		: m_Status(HTTP_OK)
		, m_strReason(GetReasonForStatus(HTTP_OK))
	{

	}

	HTTPResponse::HTTPResponse(HTTPStatus Status, LPCTSTR lpszReason)
		: m_Status(Status)
		, m_strReason(lpszReason)
	{

	}

	HTTPResponse::HTTPResponse(LPCTSTR lpszVersion, HTTPStatus Status, LPCTSTR lpszReason)
		: HTTPMessage(lpszVersion)
		, m_Status(Status)
		, m_strReason(lpszReason)
	{

	}


	HTTPResponse::HTTPResponse(LPCTSTR lpszVersion, HTTPStatus Status)
		: HTTPMessage(lpszVersion)
		, m_Status(Status)
		, m_strReason(GetReasonForStatus(Status))
	{

	}

	HTTPResponse::HTTPResponse(HTTPStatus Status)
		: m_Status(Status)
		, m_strReason(GetReasonForStatus(Status))
	{

	}

	HTTPResponse::HTTPResponse(const HTTPResponse& HttpResponse)
	{
		if (&HttpResponse != this)
		{
			m_Status = HttpResponse.GetStatus();
			m_strReason = HttpResponse.GetReason();
		}
	}

	HTTPResponse& HTTPResponse::operator = (const HTTPResponse& HttpResponse)
	{
		if (&HttpResponse != this)
		{
			m_Status = HttpResponse.GetStatus();
			m_strReason = HttpResponse.GetReason();
		}
		return *this;
	}

	HTTPResponse::~HTTPResponse()
	{

	}

	void HTTPResponse::SetStatus(HTTPStatus Status)
	{
		m_Status = Status;
	}

	void HTTPResponse::SetStatus(LPCTSTR lpszStatus)
	{
		SetStatus((HTTPStatus)_ttoi(lpszStatus));
	}

	HTTPResponse::HTTPStatus HTTPResponse::GetStatus() const
	{
		return m_Status;
	}

	void HTTPResponse::SetReason(LPCTSTR lpszReason)
	{
		m_strReason = lpszReason;
	}

	CString HTTPResponse::GetReason() const
	{
		return m_strReason;
	}

	void HTTPResponse::SetStatusAndReason(HTTPStatus Status, LPCTSTR lpszReason)
	{
		m_Status = Status;
		m_strReason = lpszReason;
	}

	void HTTPResponse::SetStatusAndReason(HTTPStatus Status)
	{
		SetStatusAndReason(Status, GetReasonForStatus(Status));
	}

	void HTTPResponse::SetDate(DWORD dwTimestamp)
	{
		//Set(DATE, )
	}

	DWORD HTTPResponse::GetDate() const
	{
		CString strDateTime = Get(DATE);
		return 0;
	}

	void HTTPResponse::AddCookie(const HTTPCookie& Cookie)
	{
		Add(SET_COOKIE, Cookie.ToString());
	}

	void HTTPResponse::GetCookies(std::vector<HTTPCookie>& Cookies) const
	{
		Cookies.clear();
		ConstIterator it = Find(SET_COOKIE);
		
	}

	LPCTSTR HTTPResponse::GetReasonForStatus(HTTPStatus status)
	{
		switch (status)
		{
		case HTTP_CONTINUE: 
			return HTTP_REASON_CONTINUE;
		case HTTP_SWITCHING_PROTOCOLS: 
			return HTTP_REASON_SWITCHING_PROTOCOLS;
		case HTTP_PROCESSING:
			return HTTP_REASON_PROCESSING;
		case HTTP_OK: 
			return HTTP_REASON_OK;
		case HTTP_CREATED: 
			return HTTP_REASON_CREATED;
		case HTTP_ACCEPTED: 
			return HTTP_REASON_ACCEPTED;
		case HTTP_NONAUTHORITATIVE:	
			return HTTP_REASON_NONAUTHORITATIVE;
		case HTTP_NO_CONTENT: 
			return HTTP_REASON_NO_CONTENT;
		case HTTP_RESET_CONTENT: 
			return HTTP_REASON_RESET_CONTENT;
		case HTTP_PARTIAL_CONTENT: 
			return HTTP_REASON_PARTIAL_CONTENT;
		case HTTP_MULTI_STATUS:
			return HTTP_REASON_MULTI_STATUS;
		case HTTP_ALREADY_REPORTED:
			return HTTP_REASON_ALREADY_REPORTED;
		case HTTP_IM_USED:
			return HTTP_REASON_IM_USED;
		case HTTP_MULTIPLE_CHOICES: 
			return HTTP_REASON_MULTIPLE_CHOICES;
		case HTTP_MOVED_PERMANENTLY: 
			return HTTP_REASON_MOVED_PERMANENTLY;
		case HTTP_FOUND: 
			return HTTP_REASON_FOUND;
		case HTTP_SEE_OTHER: 
			return HTTP_REASON_SEE_OTHER;
		case HTTP_NOT_MODIFIED: 
			return HTTP_REASON_NOT_MODIFIED;
		case HTTP_USE_PROXY: 
			return HTTP_REASON_USE_PROXY;
		case HTTP_TEMPORARY_REDIRECT: 
			return HTTP_REASON_TEMPORARY_REDIRECT;
		case HTTP_BAD_REQUEST: 
			return HTTP_REASON_BAD_REQUEST;
		case HTTP_UNAUTHORIZED: 
			return HTTP_REASON_UNAUTHORIZED;
		case HTTP_PAYMENT_REQUIRED: 
			return HTTP_REASON_PAYMENT_REQUIRED;
		case HTTP_FORBIDDEN: 
			return HTTP_REASON_FORBIDDEN;
		case HTTP_NOT_FOUND: 
			return HTTP_REASON_NOT_FOUND;
		case HTTP_METHOD_NOT_ALLOWED:
			return HTTP_REASON_METHOD_NOT_ALLOWED;
		case HTTP_NOT_ACCEPTABLE: 
			return HTTP_REASON_NOT_ACCEPTABLE;
		case HTTP_PROXY_AUTHENTICATION_REQUIRED: 
			return HTTP_REASON_PROXY_AUTHENTICATION_REQUIRED;
		case HTTP_REQUEST_TIMEOUT: 
			return HTTP_REASON_REQUEST_TIMEOUT;
		case HTTP_CONFLICT: 
			return HTTP_REASON_CONFLICT;
		case HTTP_GONE: 
			return HTTP_REASON_GONE;
		case HTTP_LENGTH_REQUIRED: 
			return HTTP_REASON_LENGTH_REQUIRED;
		case HTTP_PRECONDITION_FAILED: 
			return HTTP_REASON_PRECONDITION_FAILED;
		case HTTP_REQUEST_ENTITY_TOO_LARGE: 
			return HTTP_REASON_REQUEST_ENTITY_TOO_LARGE;
		case HTTP_REQUEST_URI_TOO_LONG: 
			return HTTP_REASON_REQUEST_URI_TOO_LONG;
		case HTTP_UNSUPPORTED_MEDIA_TYPE: 
			return HTTP_REASON_UNSUPPORTED_MEDIA_TYPE;
		case HTTP_REQUESTED_RANGE_NOT_SATISFIABLE: 
			return HTTP_REASON_REQUESTED_RANGE_NOT_SATISFIABLE;
		case HTTP_EXPECTATION_FAILED: 
			return HTTP_REASON_EXPECTATION_FAILED;
		case HTTP_IM_A_TEAPOT:
			return HTTP_REASON_IM_A_TEAPOT;            
		case HTTP_ENCHANCE_YOUR_CALM:
			return HTTP_REASON_ENCHANCE_YOUR_CALM;
		case HTTP_MISDIRECTED_REQUEST:
			return HTTP_REASON_MISDIRECTED_REQUEST;
		case HTTP_UNPROCESSABLE_ENTITY:
			return HTTP_REASON_UNPROCESSABLE_ENTITY;
		case HTTP_LOCKED:
			return HTTP_REASON_LOCKED;
		case HTTP_FAILED_DEPENDENCY:
			return HTTP_REASON_FAILED_DEPENDENCY;
		case HTTP_UPGRADE_REQUIRED:
			return HTTP_REASON_UPGRADE_REQUIRED;
		case HTTP_PRECONDITION_REQUIRED:
			return HTTP_REASON_PRECONDITION_REQUIRED;
		case HTTP_TOO_MANY_REQUESTS:
			return HTTP_REASON_TOO_MANY_REQUESTS;
		case HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE:
			return HTTP_REASON_REQUEST_HEADER_FIELDS_TOO_LARGE;
		case HTTP_UNAVAILABLE_FOR_LEGAL_REASONS:
			return HTTP_REASON_UNAVAILABLE_FOR_LEGAL_REASONS;
		case HTTP_INTERNAL_SERVER_ERROR: 
			return HTTP_REASON_INTERNAL_SERVER_ERROR;
		case HTTP_NOT_IMPLEMENTED: 
			return HTTP_REASON_NOT_IMPLEMENTED;
		case HTTP_BAD_GATEWAY: 
			return HTTP_REASON_BAD_GATEWAY;
		case HTTP_SERVICE_UNAVAILABLE:
			return HTTP_REASON_SERVICE_UNAVAILABLE;
		case HTTP_GATEWAY_TIMEOUT: 
			return HTTP_REASON_GATEWAY_TIMEOUT;
		case HTTP_VERSION_NOT_SUPPORTED: 
			return HTTP_REASON_VERSION_NOT_SUPPORTED;
		case HTTP_VARIANT_ALSO_NEGOTIATES:
			return HTTP_REASON_VARIANT_ALSO_NEGOTIATES;
		case HTTP_INSUFFICIENT_STORAGE:
			return HTTP_REASON_INSUFFICIENT_STORAGE;
		case HTTP_LOOP_DETECTED:
			return HTTP_REASON_LOOP_DETECTED;
		case HTTP_NOT_EXTENDED:
			return HTTP_REASON_NOT_EXTENDED;
		case HTTP_NETWORK_AUTHENTICATION_REQUIRED:
			return HTTP_REASON_NETWORK_AUTHENTICATION_REQUIRED;
		default: 
			return HTTP_REASON_UNKNOWN;
		}
	}
}