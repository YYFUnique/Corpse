#pragma once

#ifdef LS_STATIC_LIB_CALL
#define	LS_COMMON_API
#elif defined LS_EXPORTS
#define	LS_COMMON_API  __declspec(dllexport)
#else
#define	LS_COMMON_API  __declspec(dllimport)
#endif

#include < Iphlpapi.h>
#include <Ws2spi.h>
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Ws2_32.lib")

#define	TABLE_SIZE			1024

typedef CList<MIB_IPNETROW, MIB_IPNETROW &> CMibIpNetRowList;

typedef CList<WSAPROTOCOL_INFO,WSAPROTOCOL_INFO&> CWsaProtocolInfoList;

typedef CList<MIB_IPFORWARD_ROW2,MIB_IPFORWARD_ROW2&> CMibIpforwardTableList;
typedef CList<MIB_IPFORWARDROW,MIB_IPFORWARDROW&> CMibIpforwardIpv4List;

typedef struct _LOCAL_NET_CONNECTION_STATE
{
	DWORD   dwProtol;					//Э��ID
	DWORD   dwLocalIpAddr;			//����IP��ַ
	DWORD   dwLocalPort;				//���ض˿�
	DWORD   dwRemoteIpAddr;		//�Է�IP
	DWORD   dwRemotePort;			//�Է��˿�
	DWORD   dwState;						//״̬
	DWORD   dwProcessID;				//����ID
	CString		 strProcessName;		//���̻��߷�����
	CString		 strProcessAuth;			//���������û���
	CString		 strLocalProcessPath;  //���ؽ���·��
	LARGE_INTEGER	liCreateTime;//����ʱ��
}LOCAL_NET_CONNECTION_STATE, *PLOCAL_NET_CONNECTION_STATE;

typedef CList<LOCAL_NET_CONNECTION_STATE, LOCAL_NET_CONNECTION_STATE&> CLocalNetConnectionStateList;

LS_COMMON_API BOOL LsGetIpNetTable(CMibIpNetRowList& MibIpNetRowList);


/************************************************************************/
// �������ƣ�FlushDynamicArp
// �������ܣ����ϵͳ��ARP������Ϣ
// ������������
// ��  �� ֵ��ִ�гɹ�����TRUE�����򷵻�FALSE
/************************************************************************/
LS_COMMON_API BOOL FlushDynamicArp();

/************************************************************************/
// �������ƣ�GetIpString
// �������ܣ����������͵�Ip��ַת��Ϊ���ʮ����IP��ַ
// �����������������͵�IP��ַ
// ��  �� ֵ��ִ�гɹ����ض�Ӧ��IP��ַ
/************************************************************************/
LS_COMMON_API CString GetIpString(DWORD dwIPAddr);

/************************************************************************/
// �������ƣ�GetSystemNetworkProtocol
// �������ܣ���ȡϵͳ������Э��
// ����������lpiProtocols Э�����ͣ����Ϊ�գ����ȡ���е�Э��
//				     WsaProtocolInfoList ����Э�������
// ��  �� ֵ��ִ�гɹ�����TRUE�����򷵻�FALSE
/************************************************************************/
LS_COMMON_API BOOL GetSystemNetworkProtocol(LPINT lpiProtocols,CWsaProtocolInfoList& WsaProtocolInfoList);

/************************************************************************/
// �������ƣ�GetIpForwardTableIpv4
// �������ܣ���ȡϵͳIPv4·��״̬��Ϣ
// ����������MibIpforwardTableList ·������
// ��  �� ֵ��ִ�гɹ�����TRUE�����򷵻�FALSE
/************************************************************************/
LS_COMMON_API BOOL GetIpForwardTableIpv4(CMibIpforwardIpv4List& MibIpforwardIpv4List);

/************************************************************************/
// �������ƣ�GetIpForwardTableList
// �������ܣ���ȡϵͳ·��״̬��Ϣ
// ����������Family ·�ɰ汾IPv4(AF_INET)��IPv6(AF_INET6)
//				     MibIpforwardTableList ·������
// ��  �� ֵ��ִ�гɹ�����TRUE�����򷵻�FALSE
/************************************************************************/
LS_COMMON_API BOOL GetIpForwardTableList(CMibIpforwardTableList& MibIpforwardTableList,ADDRESS_FAMILY  Family = AF_UNSPEC);

LS_COMMON_API BOOL GetTcpNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList);

LS_COMMON_API BOOL GetUdpNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList);

LS_COMMON_API BOOL GetNetWorkConnectTable(CLocalNetConnectionStateList& LocalNetConnectionStateList);