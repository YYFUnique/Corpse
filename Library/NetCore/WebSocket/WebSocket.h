#pragma once

namespace NetCore
{
	typedef enum tagMode
	{
		WS_SERVER, /// Server-side WebSocket.
		WS_CLIENT  /// Client-side WebSocket.
	}WS_MODE;

	typedef enum tagFrameFlags	/// Frame header flags.
	{
		FRAME_FLAG_FIN  = 0x80, /// FIN bit: final fragment of a multi-fragment message.
		FRAME_FLAG_RSV1 = 0x40, /// Reserved for future use. Must be zero.
		FRAME_FLAG_RSV2 = 0x20, /// Reserved for future use. Must be zero.
		FRAME_FLAG_RSV3 = 0x10,  /// Reserved for future use. Must be zero.
	}FrameFlags;

	typedef enum tagFrameOpcodes
		/// Frame header opcodes.
	{
		FRAME_OP_CONT		= 0x00, /// Continuation frame.
		FRAME_OP_TEXT			= 0x01, /// Text frame.
		FRAME_OP_BINARY	= 0x02, /// Binary frame.
		FRAME_OP_CLOSE		= 0x08, /// Close connection.
		FRAME_OP_PING		= 0x09, /// Ping frame.
		FRAME_OP_PONG		= 0x0a, /// Pong frame.
		FRAME_OP_BITMASK	= 0x0f, /// Bit mask for opcodes. 
		FRAME_OP_SETRAW	= 0x100, /// Set raw flags (for use with sendBytes() and FRAME_OP_CONT).
	}FrameOpcodes;

	typedef enum tagSendFlags		/// Combined header flags and opcodes for use with sendFrame().
	{
		FRAME_TEXT   = FRAME_FLAG_FIN | FRAME_OP_TEXT,		/// Use this for sending a single text (UTF-8) payload frame.
		FRAME_BINARY = FRAME_FLAG_FIN | FRAME_OP_BINARY,	/// Use this for sending a single binary payload frame.
	}SendFlags;

	typedef enum tagStatusCodes			/// StatusCodes for CLOSE frames sent with shutdown().
	{
		WS_NORMAL_CLOSE							= 1000,
		WS_ENDPOINT_GOING_AWAY			= 1001,
		WS_PROTOCOL_ERROR						= 1002,
		WS_PAYLOAD_NOT_ACCEPTABLE		= 1003,
		WS_RESERVED										= 1004,
		WS_RESERVED_NO_STATUS_CODE	= 1005,
		WS_RESERVED_ABNORMAL_CLOSE	= 1006,
		WS_MALFORMED_PAYLOAD				= 1007,
		WS_POLICY_VIOLATION						= 1008,
		WS_PAYLOAD_TOO_BIG						= 1009,
		WS_EXTENSION_REQUIRED				= 1010,
		WS_UNEXPECTED_CONDITION			= 1011,
		WS_RESERVED_TLS_FAILURE				= 1015,
	}StatusCodes;

	/// These error codes can be obtained from a WebSocketException
	/// to determine the exact cause of the error.
	typedef enum tagErrorCodes
	{
		WS_ERR_NO_HANDSHAKE											= 1,
		/// No Connection: Upgrade or Upgrade: websocket header in handshake request.
		WS_ERR_HANDSHAKE_NO_VERSION						= 2,
		/// No Sec-WebSocket-Version header in handshake request.
		WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION		= 3,
		/// Unsupported WebSocket version requested by client.
		WS_ERR_HANDSHAKE_NO_KEY									= 4,
		/// No Sec-WebSocket-Key header in handshake request.
		WS_ERR_HANDSHAKE_ACCEPT									= 5,
		/// No Sec-WebSocket-Accept header or wrong value.
		WS_ERR_UNAUTHORIZED											= 6,
		/// The server rejected the username or password for authentication.
		WS_ERR_PAYLOAD_TOO_BIG										= 10,
		/// Payload too big for supplied buffer.
		WS_ERR_INCOMPLETE_FRAME									= 11,
		/// Incomplete frame received.
	}ErrorCodes;

	/// This class implements a WebSocket according
	/// to the WebSocket protocol specification in RFC 6455.
	///
	/// Both client-side and server-side WebSockets
	/// are supported.
	///
	/// Server-side WebSockets are usually created from within
	/// a HTTPRequestHandler.
	///
	/// Client-side WebSockets are created using a HTTPClientSession.
	///
	/// Note that special frames like PING must be handled at
	/// application level. In the case of a PING, a PONG message
	/// must be returned.
	class NET_API WebSocket : public StreamSocket
	{
	public:
		WebSocket();
	};
}