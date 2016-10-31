#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSTcpIP.h>
#pragma comment(lib,"ws2_32.lib")

#define PROTOCOL_STRING_ICMP	_T("ICMP")
#define PROTOCOL_STRING_TCP		_T("TCP")
#define PROTOCOL_STRING_UDP		_T("UDP")
#define PROTOCOL_STRING_SPX		_T("SPX")
#define PROTOCOL_STRING_NCP		_T("NCP")

typedef enum PROTOCOL_TYPE
{
	PROTOCOL_TYPE_ICMP = 1,
	PROTOCOL_TYPE_IGMP = 2,
	PROTOCOL_TYPE_TCP		= 6,
	PROTOCOL_TYPE_UDP   = 17,
}PROTOCOL_TYPE;

//����IP�ײ�
typedef struct IP_HEADER 
{
	BYTE	bLenVer;					//�汾��Ϣ(ǰ4λ)��ͷ����(��4λ)
	BYTE	bTypeOfService;		//��������8λ
	WORD wPacketLen;				//���ݰ�����
	WORD wPacketId;				//���ݰ���ʶ
	WORD	wSliceInfo;				//��Ƭʹ��
	BYTE	bTTL;						//���ʱ��
	BYTE	bProtoType;				//Э������(TCP,UDP,ICMP...)    
	WORD	wCheckSum;			//IP�ײ�У���
	DWORD dwSrcIp;
	DWORD dwDstIp;
}IP_HEADER,*PIP_HEADER;

//����TCPα�ײ�
//typedef struct 

//����TCP�ײ�
typedef struct TCP_HEADER
{

}TCP_HEADER,*PTCP_HEADER;

//����ICMP�ײ�
typedef struct ICMP_HEADER
{

}ICMP_HEADER,*PICMP_HEADER;

//����UDP�ײ�
typedef struct UDP_HEADER
{

}UDP_HEADER,*PUDP_HEADER;