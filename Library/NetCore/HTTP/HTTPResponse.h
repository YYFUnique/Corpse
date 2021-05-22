#pragma once

namespace NetCore
{
	#define HTTP_REASON_CONTINUE														_T("Continue")
	#define HTTP_REASON_SWITCHING_PROTOCOLS								_T("Switching Protocols")
	#define HTTP_REASON_PROCESSING													_T("Processing")
	#define HTTP_REASON_OK																	_T("OK")
	#define HTTP_REASON_CREATED															_T("Created")
	#define HTTP_REASON_ACCEPTED														_T("Accepted")
	#define HTTP_REASON_NONAUTHORITATIVE										_T("Non-Authoritative Information")
	#define HTTP_REASON_NO_CONTENT													_T("No Content")
	#define HTTP_REASON_RESET_CONTENT												_T("Reset Content")
	#define HTTP_REASON_PARTIAL_CONTENT											_T("Partial Content")
	#define HTTP_REASON_MULTI_STATUS												_T("Multi Status")
	#define HTTP_REASON_ALREADY_REPORTED										_T("Already Reported")
	#define HTTP_REASON_IM_USED															_T("IM Used")
	#define HTTP_REASON_MULTIPLE_CHOICES										_T("Multiple Choices")
	#define HTTP_REASON_MOVED_PERMANENTLY									_T("Moved Permanently")
	#define HTTP_REASON_FOUND															_T("Found")
	#define HTTP_REASON_SEE_OTHER														_T("See Other")
	#define HTTP_REASON_NOT_MODIFIED												_T("Not Modified")
	#define HTTP_REASON_USE_PROXY														_T("Use Proxy")
	#define HTTP_REASON_TEMPORARY_REDIRECT									_T("Temporary Redirect")
	#define HTTP_REASON_PERMANENT_REDIRECT									_T("Permanent Redirect")
	#define HTTP_REASON_BAD_REQUEST													_T("Bad Request")
	#define HTTP_REASON_UNAUTHORIZED												_T("Unauthorized")
	#define HTTP_REASON_PAYMENT_REQUIRED										_T("Payment Required")
	#define HTTP_REASON_FORBIDDEN														_T("Forbidden")
	#define HTTP_REASON_NOT_FOUND													_T("Not Found")
	#define HTTP_REASON_METHOD_NOT_ALLOWED								_T("Method Not Allowed")
	#define HTTP_REASON_NOT_ACCEPTABLE											_T("Not Acceptable")
	#define HTTP_REASON_PROXY_AUTHENTICATION_REQUIRED			_T("Proxy Authentication Required")
	#define HTTP_REASON_REQUEST_TIMEOUT											_T(	"Request Time-out")
	#define HTTP_REASON_CONFLICT														_T("Conflict")
	#define HTTP_REASON_GONE																_T("Gone")
	#define HTTP_REASON_LENGTH_REQUIRED											_T("Length Required")
	#define HTTP_REASON_PRECONDITION_FAILED									_T("Precondition Failed")
	#define HTTP_REASON_REQUEST_ENTITY_TOO_LARGE						_T("Request Entity Too Large")
	#define HTTP_REASON_REQUEST_URI_TOO_LONG								_T("Request-URI Too Large")
	#define HTTP_REASON_UNSUPPORTED_MEDIA_TYPE							_T("Unsupported Media Type")
	#define HTTP_REASON_REQUESTED_RANGE_NOT_SATISFIABLE			_T("Requested Range Not Satisfiable")
	#define HTTP_REASON_EXPECTATION_FAILED										_T("Expectation Failed")
	#define HTTP_REASON_IM_A_TEAPOT													_T("I'm a Teapot")
	#define HTTP_REASON_ENCHANCE_YOUR_CALM								_T("Enchance Your Calm")
	#define HTTP_REASON_MISDIRECTED_REQUEST									_T("Misdirected Request")
	#define HTTP_REASON_UNPROCESSABLE_ENTITY								_T("Unprocessable Entity")
	#define HTTP_REASON_LOCKED															_T("Locked")
	#define HTTP_REASON_FAILED_DEPENDENCY										_T("Failed Dependency")
	#define HTTP_REASON_UPGRADE_REQUIRED										_T("Upgrade Required")
	#define HTTP_REASON_PRECONDITION_REQUIRED							_T("Precondition Required")
	#define HTTP_REASON_TOO_MANY_REQUESTS									_T("Too Many Requests")
	#define HTTP_REASON_REQUEST_HEADER_FIELDS_TOO_LARGE			_T("Request Header Fields Too Large")
	#define HTTP_REASON_UNAVAILABLE_FOR_LEGAL_REASONS			_T("Unavailable For Legal Reasons")
	#define HTTP_REASON_INTERNAL_SERVER_ERROR								_T("Internal Server Error")
	#define HTTP_REASON_NOT_IMPLEMENTED										_T("Not Implemented")
	#define HTTP_REASON_BAD_GATEWAY												_T("Bad Gateway")
	#define HTTP_REASON_SERVICE_UNAVAILABLE									_T("Service Unavailable")
	#define HTTP_REASON_GATEWAY_TIMEOUT										_T("Gateway Time-Out")
	#define HTTP_REASON_VERSION_NOT_SUPPORTED							_T("HTTP Version Not Supported")
	#define HTTP_REASON_VARIANT_ALSO_NEGOTIATES						_T("Variant Also Negotiates")
	#define HTTP_REASON_INSUFFICIENT_STORAGE									_T("Insufficient Storage")
	#define HTTP_REASON_LOOP_DETECTED												_T("Loop Detected")
	#define HTTP_REASON_NOT_EXTENDED												_T("Not Extended")
	#define HTTP_REASON_NETWORK_AUTHENTICATION_REQUIRED		_T("Network Authentication Required")
	#define HTTP_REASON_UNKNOWN														_T("???")
	#define DATE																							_T("Date")
	#define SET_COOKIE																				_T("Set-Cookie")

	class NET_API HTTPResponse : public HTTPMessage
	{
	public:
		enum HTTPStatus
		{
			HTTP_CONTINUE													= 100,
			HTTP_SWITCHING_PROTOCOLS							= 101,
			HTTP_PROCESSING													= 102,
			HTTP_OK																	= 200,
			HTTP_CREATED														= 201,
			HTTP_ACCEPTED														= 202,
			HTTP_NONAUTHORITATIVE									= 203,
			HTTP_NO_CONTENT												= 204,
			HTTP_RESET_CONTENT											= 205,
			HTTP_PARTIAL_CONTENT										= 206,
			HTTP_MULTI_STATUS												= 207,
			HTTP_ALREADY_REPORTED									= 208,
			HTTP_IM_USED														= 226,
			HTTP_MULTIPLE_CHOICES										= 300,
			HTTP_MOVED_PERMANENTLY								= 301,
			HTTP_FOUND															= 302,
			HTTP_SEE_OTHER													= 303,
			HTTP_NOT_MODIFIED											= 304,
			HTTP_USE_PROXY													= 305,
			HTTP_USEPROXY														= 305, /// @deprecated
			// UNUSED: 306
			HTTP_TEMPORARY_REDIRECT								= 307,
			HTTP_PERMANENT_REDIRECT								= 308,
			HTTP_BAD_REQUEST												= 400,
			HTTP_UNAUTHORIZED											= 401,
			HTTP_PAYMENT_REQUIRED									= 402,
			HTTP_FORBIDDEN													= 403,
			HTTP_NOT_FOUND												= 404,
			HTTP_METHOD_NOT_ALLOWED							= 405,
			HTTP_NOT_ACCEPTABLE										= 406,
			HTTP_PROXY_AUTHENTICATION_REQUIRED		= 407,
			HTTP_REQUEST_TIMEOUT										= 408,
			HTTP_CONFLICT														= 409,
			HTTP_GONE															= 410,
			HTTP_LENGTH_REQUIRED										= 411,
			HTTP_PRECONDITION_FAILED								= 412,
			HTTP_REQUEST_ENTITY_TOO_LARGE					= 413,
			HTTP_REQUESTENTITYTOOLARGE							= 413, /// @deprecated
			HTTP_REQUEST_URI_TOO_LONG							= 414,
			HTTP_REQUESTURITOOLONG								= 414, /// @deprecated
			HTTP_UNSUPPORTED_MEDIA_TYPE						= 415,
			HTTP_UNSUPPORTEDMEDIATYPE							= 415, /// @deprecated
			HTTP_REQUESTED_RANGE_NOT_SATISFIABLE		= 416,
			HTTP_EXPECTATION_FAILED									=	417,
			HTTP_IM_A_TEAPOT												= 418,
			HTTP_ENCHANCE_YOUR_CALM								= 420,
			HTTP_MISDIRECTED_REQUEST								= 421,
			HTTP_UNPROCESSABLE_ENTITY							= 422,
			HTTP_LOCKED															= 423,
			HTTP_FAILED_DEPENDENCY									= 424,
			HTTP_UPGRADE_REQUIRED									= 426,
			HTTP_PRECONDITION_REQUIRED							= 428,
			HTTP_TOO_MANY_REQUESTS								= 429,
			HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE		= 431,
			HTTP_UNAVAILABLE_FOR_LEGAL_REASONS			= 451,
			HTTP_INTERNAL_SERVER_ERROR							= 500,
			HTTP_NOT_IMPLEMENTED										= 501,
			HTTP_BAD_GATEWAY												= 502,
			HTTP_SERVICE_UNAVAILABLE								= 503,
			HTTP_GATEWAY_TIMEOUT									= 504,
			HTTP_VERSION_NOT_SUPPORTED						= 505,
			HTTP_VARIANT_ALSO_NEGOTIATES						= 506,
			HTTP_INSUFFICIENT_STORAGE								= 507,
			HTTP_LOOP_DETECTED											= 508,
			HTTP_NOT_EXTENDED											= 510,
			HTTP_NETWORK_AUTHENTICATION_REQUIRED	= 511
		};
	public:
		HTTPResponse();
		HTTPResponse(HTTPStatus Status, LPCTSTR lpszReason);
		HTTPResponse(LPCTSTR lpszVersion, HTTPStatus Status, LPCTSTR lpszReason);
		HTTPResponse(HTTPStatus Status);
		HTTPResponse(LPCTSTR lpszVersion, HTTPStatus Status);

		~HTTPResponse();

	public:
		void SetStatus(HTTPStatus Status);
		HTTPStatus GetStatus() const;
		void SetStatus(LPCTSTR lpszStatus);
		
		void SetReason(LPCTSTR lpszReason);
		CString GetReason() const;

		void SetStatusAndReason(HTTPStatus Status, LPCTSTR lpszReason);
		void SetStatusAndReason(HTTPStatus Status);

		void SetDate(DWORD dwTimestamp);
		DWORD GetDate() const;

		void AddCookie(const HTTPCookie& Cookie);
		void GetCookies(std::vector<HTTPCookie>& Cookies) const;

		static LPCTSTR GetReasonForStatus(HTTPStatus Status);
	protected:
		HTTPResponse(const HTTPResponse&);
		HTTPResponse& operator = (const HTTPResponse&);
	protected:
		HTTPStatus m_Status;
		CString			m_strReason;
	};
}