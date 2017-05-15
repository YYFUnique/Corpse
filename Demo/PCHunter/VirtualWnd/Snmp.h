#pragma once

#include "../Utils/SnmpMgr.h"

typedef enum tagGETOIDTYPE
{
	GETOIDTYPE_ONLY		= 0,
	GETOIDTYPE_GROUP	= 1,
	GETOIDTYPE_ALL		= 2,
}GETOIDTYPE;

class CSnmp : public CBase,public IRecvCallback
{
public:
	CSnmp();
	~CSnmp();

public:
	virtual void Notify(TNotifyUI& msg);
	virtual void OnPaint();
	virtual void OnClick(TNotifyUI& msg);

	void SnmpMsgHandler(smiOID sOid, smiVALUE sValue);
protected:
	void OnGetPowerOn();
	void SendMsg(LPCTSTR lpszSmiOid);
	void OnGetRemoteDevSNMP();
	void GetOidValueLenAndType(smiVALUE sValue,LPTSTR szLen,LPTSTR szType);
	static SNMPAPI_STATUS CALLBACK SnmpRecvCallback(HSNMP_SESSION hSession,HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam,LPVOID lpClientData);
protected:
	CListUI*		 m_pList;
	CString			 m_strSmiOid;
	CString			 m_strSmiOidOld;
	CSnmpMgr* m_pSnmpMgr;
	GETOIDTYPE m_GetOidType;
};