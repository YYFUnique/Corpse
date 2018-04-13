#pragma once

namespace NetCore
{
	class NET_API Socket
	{
	public:
		Socket();
		Socket(const Socket& socket);
		Socket(SocketImpl* pImpl);
		Socket& operator = (const Socket& socket);
		virtual ~Socket();

	public:
		bool operator == (const Socket& socket) const;
		bool operator != (const Socket& socket) const;
		bool operator <  (const Socket& socket) const;
		bool operator <= (const Socket& socket) const;
		bool operator >  (const Socket& socket) const;
		bool operator >= (const Socket& socket) const;

	public:
		void Init(int AF);
		/// Closes the socket.
		void Close();

		/// Returns the number of bytes available that can be read
		/// without causing the socket to block.
		int GetAvailable() const;

		/// Sets the size of the send buffer.
		void SetSendBufferSize(int size);
		
		/// Returns the size of the send buffer.
		///
		/// The returned value may be different than the  value previously set with SetSendBufferSize(),  
		/// as the system is free to adjust the value.
		int GetSendBufferSize() const;

		/// Sets the size of the receive buffer.
		void SetReceiveBufferSize(int size);
		
		/// Returns the size of the receive buffer.
		///
		/// The returned value may be different than the  value previously set with SetReceiveBufferSize(),
		/// as the system is free to adjust the value.
		int GetReceiveBufferSize() const;
		
		/// Sets the send timeout for the socket.
		void SetSendTimeout(/*const Poco::Timespan& timeout*/);

		/// Returns the send timeout for the socket.
		///
		/// The returned timeout may be different than the timeout previously set with SetSendTimeout(),
		/// as the system is free to adjust the value.
		void /*Poco::Timespan*/ GetSendTimeout() const;

		/// Sets the send timeout for the socket.
		///
		/// On systems that do not support SO_RCVTIMEO, a  workaround using poll() is provided.
		void SetReceiveTimeout(/*const Poco::Timespan& timeout*/);
		
		/// Returns the receive timeout for the socket.
		///
		/// The returned timeout may be different than the  timeout previously set with getReceiveTimeout(),
		/// as the system is free to adjust the value.
		void	/*Poco::Timespan*/ GetReceiveTimeout() const;

		/// Sets the socket option specified by level and option to the given integer value.
		void SetOption(int level, int option, int value);
		
		/// Sets the socket option specified by level and option to the given time value.
		//void SetOption(int level, int option, const IPAddress& value);
		
		/// Returns the value of the socket option specified by level and option.
		void GetOption(int level, int option, int& value) const;

		/// Sets the value of the SO_LINGER socket option.
		void SetLinger(BOOL on, int seconds); 

		/// Returns the value of the SO_LINGER socket option.
		void GetLinger(BOOL& on, int& seconds) const;
		
		/// Sets the value of the TCP_NODELAY socket option.
		void SetNoDelay(BOOL flag);
		
		/// Returns the value of the TCP_NODELAY socket option.
		BOOL IsNoDelay() const;
		
		/// Sets the value of the SO_KEEPALIVE socket option.
		void SetKeepAlive(BOOL flag);
		
		/// Returns the value of the SO_KEEPALIVE socket option.
		BOOL IsKeepAlive() const;

		/// Sets the value of the SO_REUSEADDR socket option.
		void SetReuseAddress(BOOL flag);

		/// Returns the value of the SO_REUSEADDR socket option.
		BOOL IsReuseAddress() const;
		
		/// Sets the value of the SO_REUSEPORT socket option.
		/// Does nothing if the socket implementation does not
		/// support SO_REUSEPORT.
		void SetReusePort(BOOL flag);
		
		/// Returns the value of the SO_REUSEPORT socket option.
		///
		/// Returns false if the socket implementation does not
		/// support SO_REUSEPORT.
		BOOL IsReusePort() const;
		
		/// Sets the socket in blocking mode if flag is true,
		/// disables blocking mode if flag is false.
		void SetBlocking(BOOL flag);

		/// Returns the blocking mode of the socket.
		/// This method will only work if the blocking modes of 
		/// the socket are changed via the setBlocking method!
		BOOL IsBlocking() const;
		
		/// Returns the IP address and port number of the socket.
		SocketAddress GetAddress() const;
		
		/// Returns the IP address and port number of the peer socket.
		SocketAddress GetPeerAddress() const;
		
		SocketImpl* GetImpl() const;

		BOOL IsSecure() const;
		BOOL SupportsIPv4();
		BOOL SupportsIPv6();
		poco_socket_t GetSocketFD() const;
		//bool poll(const Poco::Timespan& timeout, int mode) const;
		//static int select(SocketList& readList, SocketList& writeList, SocketList& exceptList, const Poco::Timespan& timeout);

		public:
			//static int select(SocketList& readList, SocketList& writeList, SocketList& exceptList, const Poco::Timespan& timeout);
		protected:
			SocketImpl* m_pImpl;
	};
}