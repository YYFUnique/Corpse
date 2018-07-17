#pragma once

//class TCPClient
//{
//public:
//	TCPClient(uv_loop_t* loop = uv_default_loop());
//	virtual ~TCPClient();
//	static void StartLog(const char* logpath = nullptr);//启动日志，必须启动才会生成日志
//public:
//	//基本函数
//	virtual bool connect(const char* ip, int port);//启动connect线程，循环等待直到connect完成
//	virtual bool connect6(const char* ip, int port);//启动connect线程，循环等待直到connect完成
//	virtual int  send(const char* data, std::size_t len);
//	virtual void setrecvcb(client_recvcb cb, void* userdata);////设置接收回调函数，只有一个
//	void close();
//
//	//是否启用Nagle算法
//	bool setNoDelay(bool enable);
//	bool setKeepAlive(int enable, unsigned int delay);
//
//	const char* GetLastErrMsg() const {
//		return errmsg_.c_str();
//	};
//protected:
//	//静态回调函数
//	static void AfterConnect(uv_connect_t* handle, int status);
//	static void AfterClientRecv(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf);
//	static void AfterSend(uv_write_t *req, int status);
//	static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
//	static void AfterClose(uv_handle_t *handle);
//
//	static void ConnectThread(void* arg);//真正的connect线程
//	static void ConnectThread6(void* arg);//真正的connect线程
//
//	bool init();
//	bool run(int status = UV_RUN_DEFAULT);
//private:
//	enum {
//		CONNECT_TIMEOUT,
//		CONNECT_FINISH,
//		CONNECT_ERROR,
//		CONNECT_DIS,
//	};
//	uv_tcp_t client_;//客户端连接
//	uv_loop_t *loop_;
//	uv_write_t write_req_;//写时请求
//	uv_connect_t connect_req_;//连接时请求
//	uv_thread_t connect_threadhanlde_;//线程句柄
//	std::string errmsg_;//错误信息
//	uv_buf_t readbuffer_;//接受数据的buf
//	uv_buf_t writebuffer_;//写数据的buf
//	uv_mutex_t write_mutex_handle_;//保护write,保存前一write完成才进行下一write
//
//	int connectstatus_;//连接状态
//	client_recvcb recvcb_;//回调函数
//	void* userdata_;//回调函数的用户数据
//	std::string connectip_;//连接的服务器IP
//	int connectport_;//连接的服务器端口号
//	bool isinit_;//是否已初始化，用于close函数中判断
//};