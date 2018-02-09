#pragma once

#include <Winsnmp.h>

class IRecvCallback
{
public:
	virtual void SnmpMsgHandler(smiOID sOid, smiVALUE sValue) = 0;
};

class CSnmpMgr
{
public:
	CSnmpMgr();
	~CSnmpMgr();

public:
	void SetRecvCallback(SNMPAPI_CALLBACK funCallback);
	BOOL Init(LPCTSTR lpszDstAddr, LPCTSTR lpszCommunity); 
	BOOL SendSnmpMsg(LPCTSTR lpszOidInfo, smiINT sPDUType=SNMP_PDU_GETNEXT);
	void SetOwner(IRecvCallback* pOwer);
	BOOL ValueToString(smiVALUE sValue, CString& strDevInfo);
protected:
	BOOL ReceiveMsg(); 
	static SNMPAPI_STATUS CALLBACK SnmpCallback(HSNMP_SESSION hSession,HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam,LPVOID lpClientData);

protected:
	CStringA m_strSrcAddr;
	CStringA m_strDstAddr;
	IRecvCallback* m_pRecvMsg;
	smiOCTETS m_Community;
	HSNMP_SESSION m_hSession; //会话句柄
	HSNMP_CONTEXT m_hContext; //上下文句柄
};