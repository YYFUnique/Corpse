#pragma once
#include "LsCommon/ErrorInfo.h"
#include "LsCommon/Json/JsonObject.h"
#include "Transport/socket/SocketInterface.h"
#include "Transport/socket/TcpClient.h"
#include "Transport/socket/TcpServer.h"
#include  "Transport/helper.h"

#ifdef LS_STATIC_LIB_CALL
#define NET_TRANSPORT_API
#elif defined LS_EXPORTS
#define NET_TRANSPORT_API __declspec(dllexport)
#else
#define NET_TRANSPORT_API __declspec(dllimport)
#endif

//TCP客户端端回调指针
typedef ISocketListener::EnHandleResult (*PFN_ONCONNECT)(CONNID dwConnID,LPCTSTR lpszIp,WORD wPort);
//TCP服务器端回调指针
typedef ISocketListener::EnHandleResult (*PFN_ONACCEPT)(CONNID dwConnID, UINT_PTR soClient);
//TCP回调通用函数
typedef ISocketListener::EnHandleResult (*PFN_ONSEND)(CONNID dwConnID, const BYTE* pData, int iLength);
typedef ISocketListener::EnHandleResult (*PFN_ONCLOSE)(CONNID dwConnID);
typedef ISocketListener::EnHandleResult (*PFN_ONRECEIVE)(CONNID dwConnID, const BYTE* pData, int iLength);
typedef ISocketListener::EnHandleResult (*PFN_ONERROR)(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

typedef struct _EVENT_FUNC_
{
	PFN_ONSEND			pOnSend;
	PFN_ONRECEIVE		pOnReceive;
	PFN_ONCLOSE		pOnClose;		
	PFN_ONERROR       pOnError;
	PFN_ONCONNECT	pOnConnect;	//客户端连接成功后调用
	PFN_ONACCEPT     pOnAccept;	   //客户端连接时触发
}EVENT_FUNC,*PEVENT_FUNC;

typedef struct _MSG_HEAD_
{
	DWORD dwMsgId;
	BOOL bRet;
	DWORD dwDataLen;
	TCHAR szData[1];
}MSG_HEAD,*PMSG_HEAD;

class NET_TRANSPORT_API CMsgHead
{
public:
	//==================================
	//函 数 名: CMsgHead
	//功    能: MsgHead构造函数
	//输入参数: dwMsgId消息ID
	//				  lpData 消息内容
	//返 回 值: 无返回值
	//==================================
	CMsgHead(DWORD dwMsgId,LPCTSTR lpData);

	//==================================
	//函 数 名: CMsgHead
	//功    能: MsgHead构造函数
	//输入参数: pMsg 消息结构体指针
	//返 回 值: 无返回值
	//==================================
	CMsgHead(PMSG_HEAD pMsg);

	//==================================
	//函 数 名: CMsgHead
	//功    能: MsgHead构造函数
	//输入参数: MsgHead对象指针
	//返 回 值: 无返回值
	//==================================
	CMsgHead(CMsgHead* pMsg);

	~CMsgHead();
	//==================================
	//函 数 名: GetData
	//功    能: 获取消息内容
	//输入参数: 无
	//返 回 值: 消息内容字符串引用,目前未定义
	//==================================
	CString& GetData();

	//==================================
	//函 数 名: GetMsgValue
	//功    能: 获取消息内容
	//输入参数: 无
	//返 回 值: 消息内容字符串
	//==================================
	CString GetMsgValue();

	//==================================
	//函 数 名: GetMsgId
	//功    能: 获取消息对应ID
	//输入参数: 无
	//返 回 值: 消息ID
	//==================================
	DWORD GetMsgId();

	//==================================
	//函 数 名: GetMsgHead
	//功    能: 获取消息结构体
	//输入参数: 无
	//返 回 值: 返回消息结构体
	//==================================
	PMSG_HEAD GetMsgHead();

	//==================================
	//函 数 名: GetDataLen
	//功    能: 获取消息结构体的长度，包括数据内容长度
	//输入参数: 无
	//返 回 值: 返回整个结构体的长度
	//==================================
	DWORD GetDataLen();

	//==================================
	//函 数 名: GetMsgHeadLen
	//功    能: 获取消息内容长度
	//输入参数: 无
	//返 回 值: 返回消息内容长度
	//==================================
	DWORD GetMsgHeadLen();

	//==================================
	//函 数 名: SetData
	//功    能: 设置消息内容
	//输入参数: 无
	//返 回 值: 执行成功返回TRUE
	//==================================
	BOOL SetData(LPCTSTR lpData);
private:
	CString m_strData;			//消息内容
	DWORD m_dwMsgId;		//消息编号
	PMSG_HEAD m_pMsg;	//消息结构体指针
};

class NET_TRANSPORT_API CWinSocket :public CTcpClientListener , public CTcpServerListener
{
public:
	CWinSocket();
	virtual ~CWinSocket();

	//==================================
	//函 数 名: GetConnIPAddr
	//功    能: 获取连接的对应的IP地址
	//输入参数: 无
	//返 回 值: 返回IP地址的字符串引用
	//==================================
	CString& GetConnIPAddr();

	//==================================
	//函 数 名: GetConnPort
	//功    能: 获取连接的对应的端口
	//输入参数: 无
	//返 回 值: 返回连接对应的端口
	//==================================
	WORD GetConnPort();

	//==================================
	//函 数 名: Start
	//功    能: 连接远程套接字
	//输入参数: pszRemoteAddress 远程IP地址
	//				   usPortt   远程端口
	//返 回 值: 执行成功返回TRUE，否则返回FALSE
	//==================================
	BOOL Start(LPCTSTR pszRemoteAddress,USHORT usPortt);

	//==================================
	//函 数 名: Stop
	//功    能: 终止当前连接
	//输入参数: 无
	//返 回 值: 执行成功返回TRUE，否则返回FALSE
	//==================================
	BOOL Stop();

	//==================================
	//函 数 名: RegEventFunc
	//功    能: 注册回调函数
	//输入参数: 回调函数结构体指针
	//返 回 值: 执行成功返回TRUE，否则返回FALSE
	//==================================
	BOOL RegEventFunc(PEVENT_FUNC EventFunc);
private:
	//==================================
	//函 数 名: OnSend
	//功     能: 发送数据后调用，如果注册EVENT_FUNC::OnSend
	//				回调函数，框架会自动调用该函数
	//输入参数: 系统自动传递
	//				dwConnID 当前连接编号
	//				pData        发送数据内容
	//				iLength	  发送数据内容长度
	//返 回 值: 返回ISocketListener::EnHandleResult枚举类型
	//==================================
	virtual ISocketListener::EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength);

	//==================================
	//函 数 名: OnReceive
	//功     能: 连接建立以后，如果收到数据包，并且如果注册EVENT_FUNC::OnReceive
	//				回调函数，框架会自动调用该函数
	//输入参数: 系统自动传递
	//				dwConnID 当前连接编号
	//				pData        发送数据内容
	//				iLength	  发送数据内容长度
	//返 回 值: 返回ISocketListener::EnHandleResult枚举类型
	//==================================
	virtual ISocketListener::EnHandleResult OnReceive(CONNID dwConnID, const BYTE* pData, int iLength);

	//==================================
	//函 数 名: OnClose
	//功     能: 连接建立以后，如果关闭连接，并且如果注册EVENT_FUNC::OnClose
	//				回调函数，框架会自动调用该函数
	//输入参数: 系统自动传递
	//				dwConnID 当前连接编号
	//返 回 值: 返回ISocketListener::EnHandleResult枚举类型
	//==================================
	virtual ISocketListener::EnHandleResult OnClose(CONNID dwConnID);

	//==================================
	//函 数 名: OnError
	//功     能: 如果在使用框架的时候出现错误，并且如果注册EVENT_FUNC::OnError
	//				回调函数，框架会自动调用该函数
	//输入参数: 系统自动传递
	//				dwConnID 当前连接编号
	//				enOperation 操作类型 EnSocketOperation枚举类型
	//				iErrorCode  操作的错误编号
	//返 回 值: 返回ISocketListener::EnHandleResult枚举类型
	//==================================
	virtual ISocketListener::EnHandleResult OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

	//==================================
	//函 数 名: OnConnect
	//功     能: 如果客户端连接服务端成功，并且如果注册EVENT_FUNC::OnConnect
	//				回调函数，框架会自动调用该函数
	//输入参数: 系统自动传递
	//				dwConnID 当前连接编号
	//返 回 值: 返回ISocketListener::EnHandleResult枚举类型
	//==================================
	virtual ISocketListener::EnHandleResult OnConnect(CONNID dwConnID);

	//==================================
	//函 数 名: OnAccept
	//功     能: 如果服务端程序开启监听后接收到客户端的连接，并且如果注册EVENT_FUNC::OnAccept
	//				回调函数，框架会自动调用该函数
	//输入参数: 系统自动传递
	//				dwConnID 当前连接编号
	//				soClient 当前连接套接字
	//返 回 值: 返回ISocketListener::EnHandleResult枚举类型
	//==================================
	virtual ISocketListener::EnHandleResult OnAccept(CONNID dwConnID, UINT_PTR soClient);
private:
	EVENT_FUNC m_EventFunc;		//回调函数结构体
	CString m_strIPAddr;					//连接地址
	WORD  m_wPort;							//连接端口
};

class NET_TRANSPORT_API CClientSocket : public CWinSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();

	BOOL Start(LPCTSTR pszRemoteAddress, USHORT usPortt, BOOL bAsyncConnect = FALSE);
	BOOL Stop();
	BOOL Send(CONNID ConnId,PMSG_HEAD pMsgHead,DWORD dwLen);
	BOOL Send(CONNID ConnId,const BYTE* pBuffer, int iLength);
	BOOL Send(CONNID ConnId,LPCTSTR lpData,int iLength);
	BOOL Send(CONNID ConnId,CString& strData);
	BOOL Release();
	EnServiceState GetConnStat();
private:
	
protected:
	CTcpClient m_TcpClient;
};

class NET_TRANSPORT_API CServerSocket : public CWinSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();

	BOOL Start(LPCTSTR pszBindAddress, USHORT usPortt);
	BOOL Stop();
	BOOL Send(CONNID ConnId,PMSG_HEAD pMsgHead,DWORD dwLen);
	BOOL Send(CONNID ConnId,const BYTE* pBuffer, int iLength);
	BOOL Send(CONNID ConnId,LPCTSTR lpData,int iLength);
	BOOL Send(CONNID ConnId,CString& strData);
	BOOL Release();
	EnServiceState GetConnStat();
	BOOL GetClientAddress(CONNID dwConnID, LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);
protected:
	CTcpServer m_TcpServer;
};
