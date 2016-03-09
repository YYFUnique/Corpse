/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    nuiouser.h

Abstract:

    Constants and types to access the NDISPROT driver.
    Users must also include ntddndis.h

Environment:

    User/Kernel mode.

Revision History:

    arvindm     4/12/2000    Created

--*/

#pragma once

#ifndef _NTDDK_
#include <WinIoCtl.h>
#endif

#define FSCTL_NDISPROT_BASE      FILE_DEVICE_NETWORK

#define _NDISPROT_CTL_CODE(_Function, _Method, _Access)  \
            CTL_CODE(FSCTL_NDISPROT_BASE, _Function, _Method, _Access)

#define IOCTL_NDISPROT_OPEN_DEVICE   \
            _NDISPROT_CTL_CODE(0x200, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_QUERY_OID_VALUE   \
            _NDISPROT_CTL_CODE(0x201, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_SET_OID_VALUE   \
            _NDISPROT_CTL_CODE(0x205, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_QUERY_BINDING   \
            _NDISPROT_CTL_CODE(0x203, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_BIND_WAIT   \
            _NDISPROT_CTL_CODE(0x204, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_NDISPROT_INDICATE_STATUS   \
            _NDISPROT_CTL_CODE(0x206, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�����ն˷�����IP
#define IOCTL_NDISPROT_SET_LANGUARD_SERVER_IP   \
            _NDISPROT_CTL_CODE(0x207, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//���ñ���������IP��Ϣ
#define IOCTL_NDISPROT_SET_LOCAL_ADAPTER_IP   \
            _NDISPROT_CTL_CODE(0x208, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�����Ƿ�������ARP��
#define IOCTL_NDISPROT_SET_ENABLE_CAPTURE_ARP   \
            _NDISPROT_CTL_CODE(0x209, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�����Ƿ�������й���δ��װ����
#define IOCTL_NDISPROT_SET_ENABLE_ATTACK_HOST  \
            _NDISPROT_CTL_CODE(0x20A, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//��ս��й���δ��װ����
#define IOCTL_NDISPROT_RESET_ATTACK_HOST  \
            _NDISPROT_CTL_CODE(0x20B, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�Ƴ����й���δ��װ����
#define IOCTL_NDISPROT_REMOVE_ATTACK_HOST  \
            _NDISPROT_CTL_CODE(0x20C, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//���ӱ�����������
#define IOCTL_NDISPROT_ADD_ATTACK_HOST  \
            _NDISPROT_CTL_CODE(0x20D, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//����VRV��IP�б�
#define IOCTL_NDISPROT_SET_VRV_IP \
			_NDISPROT_CTL_CODE(0x20E, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//����VRV��DNS�б�
#define IOCTL_NDISPROT_SET_VRV_DNS_NAME \
			_NDISPROT_CTL_CODE(0x20F, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�����ն˷�����MAC��Ϣ
#define IOCTL_NDISPROT_SET_LANGUARD_SERVER_MAC \
			_NDISPROT_CTL_CODE(0x210, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//���ò��񵽵�ARP�����б�
#define IOCTL_NDISPROT_RESET_ARP_REQUEST_LIST \
			_NDISPROT_CTL_CODE(0x211, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//��ȡ��ǰ���񵽵�ARP�����
#define IOCTL_NDISPROT_GET_ARP_REQUEST_LIST \
			_NDISPROT_CTL_CODE(0x212, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//���õ�ǰ·������IP��ַ
#define IOCTL_NDISPROT_SET_GATEWAY_IP \
			_NDISPROT_CTL_CODE(0x213, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//���õ�ǰ·������MAC��ַ
#define IOCTL_NDISPROT_SET_GATEWAY_MAC \
			_NDISPROT_CTL_CODE(0x214, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�������ڹ���������
#define IOCTL_NDISPROT_SET_ATTACK_ADAPTER_BY_MAC_ADDR \
			_NDISPROT_CTL_CODE(0x215, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//���ARPReply��������
#define IOCTL_NDISPROT_RESET_ARP_REPLY_LIST \
			_NDISPROT_CTL_CODE(0x216, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//��ȡARPReply��������
#define IOCTL_NDISPROT_GET_ARP_REPLY_LIST \
			_NDISPROT_CTL_CODE(0x217, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�������ݰ�ͳ�ƹ����Ƿ���(�ܿ���)
#define IOCTL_NDISPROT_ENABLE_PACKET_STATISTICS \
			_NDISPROT_CTL_CODE(0x218, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//����(����)���ݰ�ͳ�ƹ���
#define IOCTL_NDISPROT_SET_PACKET_STATISTICS_RULE \
			_NDISPROT_CTL_CODE(0x219, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//ɾ��ָ�����ݰ�ͳ�ƹ���
#define IOCTL_NDISPROT_DELETE_PACKET_STATISTICS_RULE \
			_NDISPROT_CTL_CODE(0x21A, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//ɾ���������ݰ�ͳ�ƹ���
#define IOCTL_NDISPROT_DELETE_ALL_PACKET_STATISTICS_RULE \
			_NDISPROT_CTL_CODE(0x21B, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�����Ƿ���ָ�������ݰ�ͳ�ƹ���
#define IOCTL_NDISPROT_ENABLE_PACKET_STATISTICS_RULE \
			_NDISPROT_CTL_CODE(0x21C, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//��ȡָ�������ݰ�ͳ�ƹ�����
#define IOCTL_NDISPROT_GET_PACKET_STATISTICS_RESULT \
			_NDISPROT_CTL_CODE(0x21D, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//����ָ�������ݰ�ͳ�ƹ�����
#define IOCTL_NDISPROT_RESET_PACKET_STATISTICS_RESULT \
			_NDISPROT_CTL_CODE(0x21E, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ===================== ���ͱ��������� IP ��ַ���ݰ�ͳ����ؿ��ƴ��� ======================
// ���÷��ͱ��������� IP ��ַ���ݰ�����
#define IOCTL_NDISPROT_SET_SEND_CHEAT_IP_STATISTICS_SWITCH \
            _NDISPROT_CTL_CODE(0x21F, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ��ȡ���ͱ��������� IP ��ַ���ݰ�����
#define IOCTL_NDISPROT_GET_SEND_CHEAT_IP_STATISTICS_SWITCH \
            _NDISPROT_CTL_CODE(0x220, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ��ȡ���ͱ��������� IP ��ַ���ݰ�ͳ������
#define IOCTL_NDISPROT_GET_SEND_CHEAT_IP_STATISTICS_RESULT \
            _NDISPROT_CTL_CODE(0x221, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ===================== ���ͷ��ͱ��� ARP ��ƭ���ݰ�ͳ����ؿ��ƴ��� =======================
// ���÷��ͱ��� ARP ��ƭ���ݰ�����
#define IOCTL_NDISPROT_SET_SEND_CHEAT_ARP_STATISTICS_SWITCH \
            _NDISPROT_CTL_CODE(0x222, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ��ȡ���ͱ��� ARP ��ƭ���ݰ�����
#define IOCTL_NDISPROT_GET_SEND_CHEAT_ARP_STATISTICS_SWITCH \
            _NDISPROT_CTL_CODE(0x223, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ��ȡ���ͱ��� ARP ��ƭ���ݰ�ͳ������
#define IOCTL_NDISPROT_GET_SEND_CHEAT_ARP_STATISTICS_RESULT \
            _NDISPROT_CTL_CODE(0x224, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ===================== ��ȡ������ ARP �仯���ݰ�ͳ����ؿ��ƴ��� =========================
// ������ȡ������ ARP �仯���ݰ�����
#define IOCTL_NDISPROT_SET_RECV_GATEWAY_ARP_CHANGE_STATISTICS_SWITCH \
            _NDISPROT_CTL_CODE(0x225, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ��ȡ��ȡ������ ARP �仯���ݰ�����
#define IOCTL_NDISPROT_GET_RECV_GATEWAY_ARP_CHANGE_STATISTICS_SWITCH \
            _NDISPROT_CTL_CODE(0x226, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ��ȡ���ͱ��������� IP ��ַ���ݰ�ͳ������
#define IOCTL_NDISPROT_GET_RECV_GATEWAY_ARP_CHANGE_STATISTICS_RESULT \
            _NDISPROT_CTL_CODE(0x227, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ==================== �յ����������� IP ��ͻ���ݰ�ͳ����ؿ��ƴ��� =======================
// �����յ����������� IP ��ͻ���ݰ�����
#define IOCTL_NDISPROT_SET_RECV_IP_CONFLICT_STATISTICS_SWITCH \
            _NDISPROT_CTL_CODE(0x228, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ��ȡ�յ����������� IP ��ͻ���ݰ�����
#define IOCTL_NDISPROT_GET_RECV_IP_CONFLICT_STATISTICS_SWITCH \
            _NDISPROT_CTL_CODE(0x229, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

// ��ȡ�յ����������� IP ��ͻ���ݰ�ͳ������
#define IOCTL_NDISPROT_GET_RECV_IP_CONFLICT_STATISTICS_RESULT \
            _NDISPROT_CTL_CODE(0x22A, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//��̬������
#define IOCTL_NDISPROT_BING_NETWORK_ADAPTER \
			_NDISPROT_CTL_CODE(0x22B, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//�������ݰ�
#define IOCTL_NDISPROT_SEND_RAW_PACKET \
			_NDISPROT_CTL_CODE(0x22C, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#pragma  pack (push,1) 

typedef struct _LS_HOST_ADDR_INFO
{
	ULONG					dwIp;//IP��ַ
	UCHAR					MacAddress[6];//�����ַ
}LS_HOST_ADDR_INFO, *PLS_HOST_ADDR_INFO;

#pragma pack(pop) 

typedef ULONG NDIS_OID, *PNDIS_OID;
//
//  Structure to go with IOCTL_NDISPROT_QUERY_OID_VALUE.
//  The Data part is of variable length, determined by
//  the input buffer length passed to DeviceIoControl.
//
typedef struct _NDISPROT_QUERY_OID
{
    NDIS_OID        Oid;
    UCHAR           Data[sizeof(ULONG)];

} NDISPROT_QUERY_OID, *PNDISPROT_QUERY_OID;

//
//  Structure to go with IOCTL_NDISPROT_SET_OID_VALUE.
//  The Data part is of variable length, determined
//  by the input buffer length passed to DeviceIoControl.
//
typedef struct _NDISPROT_SET_OID
{
    NDIS_OID        Oid;
    UCHAR           Data[sizeof(ULONG)];

} NDISPROT_SET_OID, *PNDISPROT_SET_OID;


//
//  Structure to go with IOCTL_NDISPROT_QUERY_BINDING.
//  The input parameter is BindingIndex, which is the
//  index into the list of bindings active at the driver.
//  On successful completion, we get back a device name
//  and a device descriptor (friendly name).
//
typedef struct _NDISPROT_QUERY_BINDING
{
    ULONG            BindingIndex;        // 0-based binding number
    ULONG            DeviceNameOffset;    // from start of this struct
    ULONG            DeviceNameLength;    // in bytes
    ULONG            DeviceDescrOffset;    // from start of this struct
    ULONG            DeviceDescrLength;    // in bytes

} NDISPROT_QUERY_BINDING, *PNDISPROT_QUERY_BINDING;

//
//  Structure to go with IOCTL_NDISPROT_INDICATE_STATUS.
//  NDISPROT copies the status indicated by the NIC and
//  also the data indicated in the StatusBuffer.
//
typedef struct _NDISPROT_INDICATE_STATUS
{
    ULONG            IndicatedStatus;        // NDIS_STATUS
    ULONG            StatusBufferOffset;    // from start of this struct
    ULONG            StatusBufferLength;    // in bytes
} NDISPROT_INDICATE_STATUS, *PNDISPROT_INDICATE_STATUS;


