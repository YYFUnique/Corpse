#include "StdAfx.h"
#include "SnmpMgr.h"
#include "../Utils/AdapterInfo.h"
#include <atltime.h>
#pragma comment(lib,"Wsnmp32.lib")

CSnmpMgr::CSnmpMgr()
{
	m_pRecvMsg = NULL;
	m_Community.ptr = NULL;
}

CSnmpMgr::~CSnmpMgr()
{
	SnmpFreeContext(m_hContext);
	SnmpClose(m_hSession);
	SnmpCleanup();
	if (m_Community.ptr != NULL)
	{
		delete[] m_Community.ptr;
		m_Community.ptr = NULL;
	}
	
}

void CSnmpMgr::SetOwner(IRecvCallback* pOwer)
{
	m_pRecvMsg = pOwer;
}

BOOL CSnmpMgr::Init(LPCTSTR lpszDstAddr, LPCTSTR lpszCommunity)
{
	BOOL bSuccess = FALSE;
	HSNMP_ENTITY hEntity = NULL;
	smiUINT32 nMajorVersion,nMinorVersion,nLevel,nTranslateMode,nRetransmitMode;

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2,2);
	if (WSAStartup(wVersionRequested,&wsaData) != ERROR_SUCCESS)
		return FALSE;

	do 
	{
		m_strDstAddr = CStringA(lpszDstAddr);
		/*m_strSrcAddr = "192.168.1.51";*/
		/*inet_ntoa()*/
		DWORD dwDstIP = inet_addr(m_strDstAddr);

		DWORD dwBestSrcIP = GetBestSourceIp(dwDstIP);
		if (dwBestSrcIP == 0)
			break;

		CDuiString strBestSrcIP;
		if (LsIpv4AddressToString(dwBestSrcIP,strBestSrcIP) == FALSE)
			break;

		m_strSrcAddr = CStringA(strBestSrcIP);
		CStringA strCommunity(lpszCommunity);
		m_Community.len = strCommunity.GetLength();
		m_Community.ptr = new BYTE[m_Community.len];
		memcpy(m_Community.ptr,strCommunity,m_Community.len);

		SNMPAPI_STATUS Status = SnmpStartup(&nMajorVersion,&nMinorVersion,&nLevel,&nTranslateMode,&nRetransmitMode);
		if (Status == SNMPAPI_FAILURE)
			break;

		if (SnmpSetTranslateMode(nTranslateMode) == SNMPAPI_FAILURE)
			break;

		if (SnmpSetRetransmitMode(nRetransmitMode) == SNMPAPI_FAILURE)
			break;

		//创建会话
		m_hSession=SnmpCreateSession(NULL, NULL, SnmpCallback, (LPVOID)this);
		if (m_hSession == SNMPAPI_FAILURE)
			break;

		//建立实体
		hEntity =SnmpStrToEntity(m_hSession, m_strDstAddr);
		if (hEntity == SNMPAPI_FAILURE)
			break;

		//建立上下文句柄
		m_hContext=SnmpStrToContext(m_hSession,&m_Community);
		if (m_hContext == SNMPAPI_FAILURE)
			break;

		//设置超时时间
		if (SnmpSetTimeout(hEntity,10)==SNMPAPI_FAILURE)
			break;

		//设置重传次数
		if (SnmpSetRetry(hEntity,1)==SNMPAPI_FAILURE)
			break;

		bSuccess = TRUE;
	} while (FALSE);
	
	if (hEntity != NULL)
	{
		SnmpFreeEntity(hEntity);
		hEntity = NULL;
	}

	if (m_Community.ptr != NULL)
	{
		delete[] m_Community.ptr;
		m_Community.ptr = NULL;
	}

	WSACleanup();

	return bSuccess;
}

BOOL CSnmpMgr::SendSnmpMsg(LPCTSTR lpszOidInfo,smiINT sPDUType)
{
	BOOL bSuccess = FALSE;

	HSNMP_VBL hVbl = SNMPAPI_FAILURE;
	HSNMP_PDU hPdu = SNMPAPI_FAILURE;
	
	HSNMP_ENTITY hSrcEntity = SNMPAPI_FAILURE ,hDestEntity = SNMPAPI_FAILURE;
	do 
	{
		//创建源主机和目的主机实体句柄
		hSrcEntity = SnmpStrToEntity(m_hSession, m_strSrcAddr);
		hDestEntity = SnmpStrToEntity(m_hSession, m_strDstAddr);
		//创建变量绑定列表
		hVbl = SnmpCreateVbl(m_hSession,NULL,NULL);
		if (hVbl == SNMPAPI_FAILURE)
			break;
		//点分十进制串转换成二进制格式
		//for(strvec::const_iterator it=strOIDArray.begin();it!=strOIDArray.end();it++)
		CStringA strOidInfo(lpszOidInfo);
		smiOID sOid;
		SnmpStrToOid(strOidInfo,&sOid);
		SnmpSetVb(hVbl,0,&sOid,NULL);

		//将数据转换成特定PDU格式
		hPdu = SnmpCreatePdu(m_hSession,sPDUType,0,NULL,NULL,hVbl);
		if (hPdu == SNMPAPI_FAILURE)
			break;

		//发送PDU
		if(SnmpSendMsg(m_hSession,hSrcEntity,hDestEntity,m_hContext,hPdu) == SNMPAPI_FAILURE)
			break;

		bSuccess = TRUE;
	} while (FALSE);

	//释放句柄
	if (hSrcEntity != SNMPAPI_FAILURE)
	{
		SnmpFreeEntity(hSrcEntity);
		hSrcEntity = SNMPAPI_FAILURE;
	}

	if (hDestEntity != SNMPAPI_FAILURE)
	{
		SnmpFreeEntity(hDestEntity);
		hDestEntity = SNMPAPI_FAILURE;
	}

	if (hPdu != SNMPAPI_FAILURE)
	{
		SnmpFreePdu(hPdu);
		hPdu = SNMPAPI_FAILURE;
	}
	
	if (hVbl != SNMPAPI_FAILURE)
	{
		SnmpFreeVbl(hVbl);
		hVbl = SNMPAPI_FAILURE;
	}

	return bSuccess;	
}

BOOL CSnmpMgr::ReceiveMsg()
{
	HSNMP_ENTITY hSrcEntity;
	HSNMP_ENTITY hDestEntity;
	HSNMP_CONTEXT hContext;
	HSNMP_PDU hPdu;
	HSNMP_VBL hVbl;
	smiOID sOIDRecv;
	smiVALUE sValue;

	do 
	{
		//接收到消息
		if (SnmpRecvMsg(m_hSession,&hSrcEntity,&hDestEntity,&hContext,&hPdu) == SNMPAPI_FAILURE)
			break;

		smiINT PDU_type;
		smiINT error_status;
		smiINT error_index;
		//提取PDU中的数据
		if(SnmpGetPduData(hPdu,&PDU_type,NULL,&error_status,&error_index,&hVbl) == SNMPAPI_FAILURE)
			break;
		int iCount = SnmpCountVbl(hVbl);
		if (SnmpGetVb(hVbl,1,&sOIDRecv,&sValue) == SNMPAPI_FAILURE)
			break;

		m_pRecvMsg->SnmpMsgHandler(sOIDRecv, sValue);
		/*CString strDevInfo;
		ValueToString(sValue, strDevInfo);
		SnmpFreeVbl(hVbl);*/
	} while (FALSE);

	return TRUE;
}

SNMPAPI_STATUS CSnmpMgr::SnmpCallback(HSNMP_SESSION hSession,HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam,LPVOID lpClientData)
{
	CSnmpMgr* pSnmpMgr = (CSnmpMgr*)lpClientData;
	pSnmpMgr->ReceiveMsg();
	return SNMPAPI_SUCCESS;
}

BOOL CSnmpMgr::ValueToString(smiVALUE sValue, CString& strDevInfo)
{
	switch(sValue.syntax)
	{
	case SNMP_SYNTAX_NSAPADDR:
	case SNMP_SYNTAX_IPADDR:
		strDevInfo.Format(_T("%d.%d.%d.%d"),sValue.value.string.ptr[0],sValue.value.string.ptr[1],sValue.value.string.ptr[2],sValue.value.string.ptr[3]);
		break;
	case SNMP_SYNTAX_OPAQUE:
		{
			if (sValue.value.string.len==0)
				strDevInfo = _T("");
		}
		break;
	case SNMP_SYNTAX_OCTETS:
		if (sValue.value.string.len==0)
			strDevInfo = _T("");
		else
		{
			memset(sValue.value.string.ptr+sValue.value.string.len,0,1);
			strDevInfo = sValue.value.string.ptr;
		}
		break;
	case SNMP_SYNTAX_TIMETICKS:
		{
			LONG lUptime;
			lUptime=sValue.value.uNumber/100;
			TCHAR szTime[MAX_PATH];
			StrFromTimeInterval(szTime, _countof(szTime), lUptime*1000, 6);
			strDevInfo = szTime;
		}
		break;
	case SNMP_SYNTAX_INT:
			strDevInfo.Format(_T("%d"),sValue.value.sNumber);
		break;
	case SNMP_SYNTAX_OID:
		{
			CHAR szOid[50];
			smiINT nLen = _countof(szOid);
			SnmpOidToStr(&sValue.value.oid, nLen, szOid);
			strDevInfo = szOid;
		}
		break;
	case SNMP_SYNTAX_UINT32:
	case SNMP_SYNTAX_CNTR32:
	case SNMP_SYNTAX_GAUGE32:
			strDevInfo.Format(_T("%u"),sValue.value.uNumber);
		break;
	default:
		strDevInfo = _T("NULL");
		break;
	}
	return TRUE;
}