#pragma once

namespace NetCore
{
	class NET_API SocketImpl : public RefCountObject
	{
		public:
			/// Get the next completed connection from the
			/// socket's completed connection queue.
			///
			/// If the queue is empty, waits until a connection
			/// request completes.
			///
			/// Returns a new TCP socket for the connection
			/// with the client.
			///
			/// The client socket's address is returned in clientAddr.
			virtual SocketImpl* AcceptConnection(SocketAddress& ClientAddr);

			/// Initializes the socket and establishes a connection to
			/// the TCP server at the given address.
			///
			/// Can also be used for UDP sockets. In this case, no
			/// connection is established. Instead, incoming and outgoing
			/// packets are restricted to the specified address.
			virtual void Connect(const SocketAddress& Address);

			/// Initializes the socket, sets the socket timeout and
			/// establishes a connection to the TCP server at the given address.
			//virtual void connect(const SocketAddress& address, const Poco::Timespan& timeout);

			/// Initializes the socket and establishes a connection to
			/// the TCP server at the given address. Prior to opening the
			/// connection the socket is set to nonblocking mode.
			virtual void ConnectNB(const SocketAddress& Address);

			/// Bind a local address to the socket.
			///
			/// This is usually only done when establishing a server
			/// socket. TCP clients should not bind a socket to a
			/// specific address.
			///
			/// If reuseAddress is true, sets the SO_REUSEADDR
			/// socket option.
			virtual void Bind(const SocketAddress& Address, BOOL ReuseAddress = FALSE);
			
			/// Bind a local address to the socket.
			///
			/// This is usually only done when establishing a server
			/// socket. TCP clients should not bind a socket to a specific address.
			///
			/// If reuseAddress is true, sets the SO_REUSEADDR socket option.
			///
			/// If reusePort is true, sets the SO_REUSEPORT socket option.
			virtual void Bind(const SocketAddress& Address, BOOL bReuseAddress, BOOL bReusePort );
			
			/// Bind a local IPv6 address to the socket.
			///
			/// This is usually only done when establishing a server
			/// socket. TCP clients should not bind a socket to a
			/// specific address.
			///
			/// If reuseAddress is true, sets the SO_REUSEADDR
			/// socket option.
			///
			/// The given address must be an IPv6 address. The
			/// IPPROTO_IPV6/IPV6_V6ONLY option is set on the socket
			/// according to the ipV6Only parameter.
			///
			/// If the library has not been built with IPv6 support,
			/// a Poco::NotImplementedException will be thrown.
			virtual void Bind6(const SocketAddress& Address, BOOL bReuseAddress = FALSE, BOOL bIPV6Only = FALSE);
			
			/// Bind a local IPv6 address to the socket.
			///
			/// This is usually only done when establishing a server
			/// socket. TCP clients should not bind a socket to a
			/// specific address.
			///
			/// If reuseAddress is true, sets the SO_REUSEADDR
			/// socket option.
			///
			/// If reusePort is true, sets the SO_REUSEPORT
			/// socket option.
			///
			/// The given address must be an IPv6 address. The
			/// IPPROTO_IPV6/IPV6_V6ONLY option is set on the socket
			/// according to the ipV6Only parameter.
			///
			/// If the library has not been built with IPv6 support,
			/// a Poco::NotImplementedException will be thrown.
			virtual void Bind6(const SocketAddress& Address, BOOL bReuseAddress, BOOL bReusePort,  BOOL bIPV6Only);
			
			/// Puts the socket into listening state.
			///
			/// The socket becomes a passive socket that can accept incoming connection requests.
			///
			/// The backlog argument specifies the maximum number of connections that can be queued  for this socket.
			virtual void Listen(int nBacklog = 64);
			
			/// Close the socket.
			virtual void Close();

			/// Shuts down the receiving part of the socket connection.
			virtual void ShutdownReceive();
			
			/// Shuts down the sending part of the socket connection.
			virtual void ShutdownSend();
			
			/// Shuts down both the receiving and the sending part of the socket connection.
			virtual void Shutdown();
			
			/// Sends the contents of the given buffer through  the socket.
			///
			/// Returns the number of bytes sent, which may be less than the number of bytes specified.
			///
			/// Certain socket implementations may also return a negative
			/// value denoting a certain condition.
			virtual int SendBytes(const void* pBbuffer, int nLen, int nFlags = 0);
			
			/// Receives data from the socket and stores it
			/// in buffer. Up to length bytes are received.
			///
			/// Returns the number of bytes received.
			///
			/// Certain socket implementations may also return a negative
			/// value denoting a certain condition.
			virtual int ReceiveBytes(void* pBuffer, int nLen, int nFlags = 0);
			
			/// Sends the contents of the given buffer through
			/// the socket to the given address.
			///
			/// Returns the number of bytes sent, which may be
			/// less than the number of bytes specified.
			virtual int SendTo(const void* pBuffer, int nLen, const SocketAddress& Address, int nFlags = 0);
			
			/// Receives data from the socket and stores it
			/// in buffer. Up to length bytes are received.
			/// Stores the address of the sender in address.
			///
			/// Returns the number of bytes received.
			virtual int ReceiveFrom(void* pBuffer, int nLen, SocketAddress& Address, int nFlags = 0);
			
			/// Sends one byte of urgent data through
			/// the socket.
			///
			/// The data is sent with the MSG_OOB flag.
			///
			/// The preferred way for a socket to receive urgent data
			/// is by enabling the SO_OOBINLINE option.
			virtual void SendUrgent(unsigned char data);

			/// Returns the number of bytes available that can be read
			/// without causing the socket to block.
			virtual int GetAvailable();

			/*virtual bool poll(const Poco::Timespan& timeout, int mode);*/
			/// Determines the status of the socket, using a
			/// call to select().
			///
			/// The mode argument is constructed by combining the values
			/// of the SelectMode enumeration.
			///
			/// Returns true if the next operation corresponding to
			/// mode will not block, false otherwise.

			/// Sets the size of the send buffer.
			virtual void SetSendBufferSize(int size);
			

			/// Returns the size of the send buffer.
			///
			/// The returned value may be different than the
			/// value previously set with setSendBufferSize(),
			/// as the system is free to adjust the value.
			virtual int GetSendBufferSize();
			
			/// Sets the size of the receive buffer.
			virtual void SetReceiveBufferSize(int size);
			
			/// Returns the size of the receive buffer.
			///
			/// The returned value may be different than the
			/// value previously set with setReceiveBufferSize(),
			/// as the system is free to adjust the value.
			virtual int GetReceiveBufferSize();
			

			//virtual void setSendTimeout(const Poco::Timespan& timeout);
			/// Sets the send timeout for the socket.

			//virtual Poco::Timespan getSendTimeout();
			/// Returns the send timeout for the socket.
			///
			/// The returned timeout may be different than the
			/// timeout previously set with setSendTimeout(),
			/// as the system is free to adjust the value.

			//virtual void setReceiveTimeout(const Poco::Timespan& timeout);
			/// Sets the send timeout for the socket.
			///
			/// On systems that do not support SO_RCVTIMEO, a
			/// workaround using poll() is provided.

			//virtual Poco::Timespan getReceiveTimeout();
			/// Returns the receive timeout for the socket.
			///
			/// The returned timeout may be different than the
			/// timeout previously set with setReceiveTimeout(),
			/// as the system is free to adjust the value.

			/// Returns the IP address and port number of the socket.
			virtual SocketAddress GetAddress();
			
			/// Returns the IP address and port number of the peer socket.
			virtual SocketAddress GetPeerAddress();

		public:
			/// Sets the socket option specified by level and option to the given integer value.
			void SetOption(int level, int option, int value);


			//void setOption(int level, int option, unsigned value);
			/// Sets the socket option specified by level and option
			/// to the given integer value.

			//void setOption(int level, int option, unsigned char value);
			/// Sets the socket option specified by level and option
			/// to the given integer value.

			//void setOption(int level, int option, const Poco::Timespan& value);
			/// Sets the socket option specified by level and option
			/// to the given time value.

			//void setOption(int level, int option, const IPAddress& value);
			/// Sets the socket option specified by level and option
			/// to the given time value.

			/// Sets the socket option specified by level and option to the given time value.
			virtual void SetRawOption(int level, int option, const void* value, int nLen);

			/// Returns the value of the socket option specified by level and option.
			void GetOption(int level, int option, int& value);

			//void getOption(int level, int option, unsigned& value);
			/// Returns the value of the socket option
			/// specified by level and option.

			//void getOption(int level, int option, unsigned char& value);
			/// Returns the value of the socket option
			/// specified by level and option.

			//void getOption(int level, int option, Poco::Timespan& value);
			/// Returns the value of the socket option
			/// specified by level and option.

			//void getOption(int level, int option, IPAddress& value);
			/// Returns the value of the socket option
			/// specified by level and option.

			/// Returns the value of the socket option specified by level and option.
			virtual void GetRawOption(int level, int option, void* value, int& nLen);

			/// Sets the value of the SO_LINGER socket option.
			void SetLinger(BOOL bLinger, int nSeconds);
			
			/// Returns the value of the SO_LINGER socket option.
			void GetLinger(BOOL& bLinger, int& nSeconds);
			
			/// Sets the value of the TCP_NODELAY socket option.
			void SetNoDelay(BOOL bFlag);
			
			/// Returns the value of the TCP_NODELAY socket option.
			BOOL IsNoDelay();
			
			/// Sets the value of the SO_KEEPALIVE socket option.
			void SetKeepAlive(BOOL bFlag);
			
			/// Returns the value of the SO_KEEPALIVE socket option.
			BOOL IsKeepAlive();
			
			/// Sets the value of the SO_REUSEADDR socket option.
			void SetReuseAddress(BOOL bFlag);
			
			/// Returns the value of the SO_REUSEADDR socket option.
			BOOL IsReuseAddress();
			
			/// Sets the value of the SO_REUSEPORT socket option.
			/// Does nothing if the socket implementation does not
			/// support SO_REUSEPORT.
			void SetReusePort(BOOL bFlag);
			
			/// Returns the value of the SO_REUSEPORT socket option.
			///
			/// Returns false if the socket implementation does not
			/// support SO_REUSEPORT.
			BOOL IsReusePort();
			
			/// Sets the value of the SO_BROADCAST socket option.
			void SetBroadcast(BOOL bFlag);
			
			/// Returns the value of the SO_BROADCAST socket option.
			BOOL IsBroadcast();
			
			/// Sets the socket in blocking mode if flag is true,
			/// disables blocking mode if flag is false.
			virtual void SetBlocking(BOOL bFlag);
			
			/// Returns the blocking mode of the socket.
			/// This method will only work if the blocking modes of
			/// the socket are changed via the setBlocking method!
			virtual BOOL IsBlocking() const;
			
			/// Returns true iff the socket's connection is secure (using SSL or TLS).
			virtual BOOL IsSecure() const;
			
			/// Returns the value of the SO_ERROR socket option.
			int GetSocketError();
			
			/// Returns the socket descriptor for the underlying native socket.
			int GetSockFD() const;
			
			/// A wrapper for the ioctl system call.
			void IOCTL(poco_ioctl_request nRequest, int& arg);
			
			/// A wrapper for the ioctl system call.
			void IOCTL(poco_ioctl_request nRequest, void* arg);
			
			/// Returns true iff the underlying socket is initialized.
			BOOL IsInitialized() const;

			/// Creates the underlying native socket.
			///
			/// Subclasses must implement this method so
			/// that it calls initSocket() with the appropriate arguments.
			///
			/// The default implementation creates a stream socket.
			virtual void Init(int AF);
		protected:
			/// Creates a SocketImpl.
			SocketImpl();

			/// Creates a SocketImpl using the given native socket.
			SocketImpl(poco_socket_t SockFD);

			virtual ~SocketImpl();
			
			/// Creates the underlying native socket.
			///
			/// The first argument, af, specifies the address family
			/// used by the socket, which should be either AF_INET or AF_INET6.
			///
			/// The second argument, type, specifies the type of the
			/// socket, which can be one of SOCK_STREAM, SOCK_DGRAM or SOCK_RAW.
			///
			/// The third argument, proto, is normally set to 0, except for raw sockets.
			void InitSocket(int AF, int nType, int nProto = 0);

			void Reset(poco_socket_t fd = INVALID_SOCKET);
			/// Allows subclasses to set the socket manually, iff no valid socket is set yet.

			static DWORD GetLastError();
		private:
			SocketImpl(const SocketImpl& );
			SocketImpl& operator = (const SocketImpl&);
		private:
			poco_socket_t m_sockfd;
			BOOL m_bLocking;
			BOOL m_bBrokenTimeout;
	};
};