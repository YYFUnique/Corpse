#pragma once

namespace NetCore
{
	class NET_API HTTPServerRequest : public HTTPRequest
	{
	public:
		HTTPServerRequest();
		~HTTPServerRequest();

	public:
		virtual const SocketAddress& GetClientAddress() const = 0;
		virtual const SocketAddress& GetServerAddress() const = 0;
		virtual const HTTPServerParams&  GetServerParams() const = 0;
		virtual const HTTPServerResponse& GetReponse() const = 0;
		virtual BOOL IsSecure() const = 0;
	};
}