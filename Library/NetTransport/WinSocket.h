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

//TCP�ͻ��˶˻ص�ָ��
typedef ISocketListener::EnHandleResult (*PFN_ONCONNECT)(CONNID dwConnID,LPCTSTR lpszIp,WORD wPort);
//TCP�������˻ص�ָ��
typedef ISocketListener::EnHandleResult (*PFN_ONACCEPT)(CONNID dwConnID, UINT_PTR soClient);
//TCP�ص�ͨ�ú���
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
	PFN_ONCONNECT	pOnConnect;	//�ͻ������ӳɹ������
	PFN_ONACCEPT     pOnAccept;	   //�ͻ�������ʱ����
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
	//�� �� ��: CMsgHead
	//��    ��: MsgHead���캯��
	//�������: dwMsgId��ϢID
	//				  lpData ��Ϣ����
	//�� �� ֵ: �޷���ֵ
	//==================================
	CMsgHead(DWORD dwMsgId,LPCTSTR lpData);

	//==================================
	//�� �� ��: CMsgHead
	//��    ��: MsgHead���캯��
	//�������: pMsg ��Ϣ�ṹ��ָ��
	//�� �� ֵ: �޷���ֵ
	//==================================
	CMsgHead(PMSG_HEAD pMsg);

	//==================================
	//�� �� ��: CMsgHead
	//��    ��: MsgHead���캯��
	//�������: MsgHead����ָ��
	//�� �� ֵ: �޷���ֵ
	//==================================
	CMsgHead(CMsgHead* pMsg);

	~CMsgHead();
	//==================================
	//�� �� ��: GetData
	//��    ��: ��ȡ��Ϣ����
	//�������: ��
	//�� �� ֵ: ��Ϣ�����ַ�������,Ŀǰδ����
	//==================================
	CString& GetData();

	//==================================
	//�� �� ��: GetMsgValue
	//��    ��: ��ȡ��Ϣ����
	//�������: ��
	//�� �� ֵ: ��Ϣ�����ַ���
	//==================================
	CString GetMsgValue();

	//==================================
	//�� �� ��: GetMsgId
	//��    ��: ��ȡ��Ϣ��ӦID
	//�������: ��
	//�� �� ֵ: ��ϢID
	//==================================
	DWORD GetMsgId();

	//==================================
	//�� �� ��: GetMsgHead
	//��    ��: ��ȡ��Ϣ�ṹ��
	//�������: ��
	//�� �� ֵ: ������Ϣ�ṹ��
	//==================================
	PMSG_HEAD GetMsgHead();

	//==================================
	//�� �� ��: GetDataLen
	//��    ��: ��ȡ��Ϣ�ṹ��ĳ��ȣ������������ݳ���
	//�������: ��
	//�� �� ֵ: ���������ṹ��ĳ���
	//==================================
	DWORD GetDataLen();

	//==================================
	//�� �� ��: GetMsgHeadLen
	//��    ��: ��ȡ��Ϣ���ݳ���
	//�������: ��
	//�� �� ֵ: ������Ϣ���ݳ���
	//==================================
	DWORD GetMsgHeadLen();

	//==================================
	//�� �� ��: SetData
	//��    ��: ������Ϣ����
	//�������: ��
	//�� �� ֵ: ִ�гɹ�����TRUE
	//==================================
	BOOL SetData(LPCTSTR lpData);
private:
	CString m_strData;			//��Ϣ����
	DWORD m_dwMsgId;		//��Ϣ���
	PMSG_HEAD m_pMsg;	//��Ϣ�ṹ��ָ��
};

class NET_TRANSPORT_API CWinSocket :public CTcpClientListener , public CTcpServerListener
{
public:
	CWinSocket();
	virtual ~CWinSocket();

	//==================================
	//�� �� ��: GetConnIPAddr
	//��    ��: ��ȡ���ӵĶ�Ӧ��IP��ַ
	//�������: ��
	//�� �� ֵ: ����IP��ַ���ַ�������
	//==================================
	CString& GetConnIPAddr();

	//==================================
	//�� �� ��: GetConnPort
	//��    ��: ��ȡ���ӵĶ�Ӧ�Ķ˿�
	//�������: ��
	//�� �� ֵ: �������Ӷ�Ӧ�Ķ˿�
	//==================================
	WORD GetConnPort();

	//==================================
	//�� �� ��: Start
	//��    ��: ����Զ���׽���
	//�������: pszRemoteAddress Զ��IP��ַ
	//				   usPortt   Զ�̶˿�
	//�� �� ֵ: ִ�гɹ�����TRUE�����򷵻�FALSE
	//==================================
	BOOL Start(LPCTSTR pszRemoteAddress,USHORT usPortt);

	//==================================
	//�� �� ��: Stop
	//��    ��: ��ֹ��ǰ����
	//�������: ��
	//�� �� ֵ: ִ�гɹ�����TRUE�����򷵻�FALSE
	//==================================
	BOOL Stop();

	//==================================
	//�� �� ��: RegEventFunc
	//��    ��: ע��ص�����
	//�������: �ص������ṹ��ָ��
	//�� �� ֵ: ִ�гɹ�����TRUE�����򷵻�FALSE
	//==================================
	BOOL RegEventFunc(PEVENT_FUNC EventFunc);
private:
	//==================================
	//�� �� ��: OnSend
	//��     ��: �������ݺ���ã����ע��EVENT_FUNC::OnSend
	//				�ص���������ܻ��Զ����øú���
	//�������: ϵͳ�Զ�����
	//				dwConnID ��ǰ���ӱ��
	//				pData        ������������
	//				iLength	  �����������ݳ���
	//�� �� ֵ: ����ISocketListener::EnHandleResultö������
	//==================================
	virtual ISocketListener::EnHandleResult OnSend(CONNID dwConnID, const BYTE* pData, int iLength);

	//==================================
	//�� �� ��: OnReceive
	//��     ��: ���ӽ����Ժ�����յ����ݰ����������ע��EVENT_FUNC::OnReceive
	//				�ص���������ܻ��Զ����øú���
	//�������: ϵͳ�Զ�����
	//				dwConnID ��ǰ���ӱ��
	//				pData        ������������
	//				iLength	  �����������ݳ���
	//�� �� ֵ: ����ISocketListener::EnHandleResultö������
	//==================================
	virtual ISocketListener::EnHandleResult OnReceive(CONNID dwConnID, const BYTE* pData, int iLength);

	//==================================
	//�� �� ��: OnClose
	//��     ��: ���ӽ����Ժ�����ر����ӣ��������ע��EVENT_FUNC::OnClose
	//				�ص���������ܻ��Զ����øú���
	//�������: ϵͳ�Զ�����
	//				dwConnID ��ǰ���ӱ��
	//�� �� ֵ: ����ISocketListener::EnHandleResultö������
	//==================================
	virtual ISocketListener::EnHandleResult OnClose(CONNID dwConnID);

	//==================================
	//�� �� ��: OnError
	//��     ��: �����ʹ�ÿ�ܵ�ʱ����ִ��󣬲������ע��EVENT_FUNC::OnError
	//				�ص���������ܻ��Զ����øú���
	//�������: ϵͳ�Զ�����
	//				dwConnID ��ǰ���ӱ��
	//				enOperation �������� EnSocketOperationö������
	//				iErrorCode  �����Ĵ�����
	//�� �� ֵ: ����ISocketListener::EnHandleResultö������
	//==================================
	virtual ISocketListener::EnHandleResult OnError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

	//==================================
	//�� �� ��: OnConnect
	//��     ��: ����ͻ������ӷ���˳ɹ����������ע��EVENT_FUNC::OnConnect
	//				�ص���������ܻ��Զ����øú���
	//�������: ϵͳ�Զ�����
	//				dwConnID ��ǰ���ӱ��
	//�� �� ֵ: ����ISocketListener::EnHandleResultö������
	//==================================
	virtual ISocketListener::EnHandleResult OnConnect(CONNID dwConnID);

	//==================================
	//�� �� ��: OnAccept
	//��     ��: �������˳�������������յ��ͻ��˵����ӣ��������ע��EVENT_FUNC::OnAccept
	//				�ص���������ܻ��Զ����øú���
	//�������: ϵͳ�Զ�����
	//				dwConnID ��ǰ���ӱ��
	//				soClient ��ǰ�����׽���
	//�� �� ֵ: ����ISocketListener::EnHandleResultö������
	//==================================
	virtual ISocketListener::EnHandleResult OnAccept(CONNID dwConnID, UINT_PTR soClient);
private:
	EVENT_FUNC m_EventFunc;		//�ص������ṹ��
	CString m_strIPAddr;					//���ӵ�ַ
	WORD  m_wPort;							//���Ӷ˿�
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
