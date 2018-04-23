#include "StdAfx.h"
#include "Snmp.h"
#include "DllCore/Utils/ErrorInfo.h"

CSnmp::CSnmp()
{
	m_pPaintManager = NULL;
	m_pSnmpMgr = NULL;
}

CSnmp::~CSnmp()
{
	if (m_pSnmpMgr != NULL)
	{
		delete m_pSnmpMgr;
		m_pSnmpMgr = NULL;
	}

	m_pPaintManager = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CSnmp, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CSnmp::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CSnmp::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_pPaintManager->FindControl(_T("BtnStart")))
		OnGetRemoteDevSNMP();
	else if (msg.pSender == m_pPaintManager->FindControl(_T("BtnPowerOn")))
		OnGetPowerOn();
}

void CSnmp::OnLoadItem(TNotifyUI& msg)
{
	if (m_pSnmpMgr != NULL)
		return;

	m_pSnmpMgr = new CSnmpMgr;
}

void CSnmp::OnGetPowerOn()
{
	COptionUI* pOid = (COptionUI*)m_pPaintManager->FindControl(_T("oid"));
	pOid->Selected(true);

	CEditUI2* pOidEdit = (CEditUI2*)m_pPaintManager->FindControl(_T("oidEdit"));
	pOidEdit->SetText(_T("1.3.6.1.2.1.1.3.0"));
}

void CSnmp::OnGetRemoteDevSNMP()
{
	BOOL bSuccess = FALSE;
	CIPAddressUI* pAddr = (CIPAddressUI*)m_pPaintManager->FindControl(_T("DevAddr"));
	CEditUI2* pPort = (CEditUI2*)m_pPaintManager->FindControl(_T("Port"));
	CEditUI2* pPassword = (CEditUI2*)m_pPaintManager->FindControl(_T("Password"));

	CString strSmiOid;
	do 
	{
		if (pAddr)
		{
			if (pAddr->GetText().IsEmpty())
			{
				SetErrorInfo(CUSTOM_ERROR, 0, _T("设备地址不能为空，请输入设备地址"));
				break;
			}
		}

		if (pPort)
		{
			if (pPort->GetText().IsEmpty())
			{
				SetErrorInfo(CUSTOM_ERROR, 0, _T("设备端口不能为空，请输入设备端口"));
				break;
			}
		}

		if (pPassword)
		{
			if (pPassword->GetText().IsEmpty())
			{
				SetErrorInfo(CUSTOM_ERROR, 0, _T("团体名不能为空，请输入团体名"));
				break;
			}
		}

		COptionUI* pOid = (COptionUI*)m_pPaintManager->FindControl(_T("oid"));
		COptionUI* pOidGroup = (COptionUI*)m_pPaintManager->FindControl(_T("oidGroup"));
		if (pOid->IsSelected()) 
		{
			m_GetOidType = GETOIDTYPE_ONLY;
			CEditUI2* pOidEdit = (CEditUI2*)m_pPaintManager->FindControl(_T("oidEdit"));
			strSmiOid = pOidEdit->GetText();
			if (strSmiOid.IsEmpty())
			{
				SetErrorInfo(CUSTOM_ERROR, 0, _T("请输入需要查询的OID值"));
				break;
			}

		} else if (pOidGroup->IsSelected())
		{
			m_GetOidType = GETOIDTYPE_GROUP;
			CEditUI2* pOidEditGroup = (CEditUI2*)m_pPaintManager->FindControl(_T("oidGroupEdit"));
			strSmiOid = pOidEditGroup->GetText();
			if (strSmiOid.IsEmpty())
			{
				SetErrorInfo(CUSTOM_ERROR, 0 ,_T("请输入需要查询的OID组值"));
				break;
			}
		} else {
			m_GetOidType = GETOIDTYPE_ALL;
			CEditUI2* pAlloid = (CEditUI2*)m_pPaintManager->FindControl(_T("alloidEdit"));
			strSmiOid = pAlloid->GetText();
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
	{
		MessageBox(m_pPaintManager->GetPaintWindow(), GetThreadErrorInfoString(), _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	m_pList = (CListUI*)m_pPaintManager->FindControl(_T("snmpList"));
	if (m_pList != NULL)
		m_pList->RemoveAll();

	if (m_pSnmpMgr != NULL)
	{
		m_pSnmpMgr->SetOwner(this);
		if (m_pSnmpMgr->Init(pAddr->GetText(), pPassword->GetText()) == FALSE)
		{
			MessageBox(m_pPaintManager->GetPaintWindow(),_T("SNMP初始化失败"),_T("提示"),MB_OK);
			return;
		}
		m_strSmiOid = strSmiOid;
		m_strSmiOidOld.Empty();
		SendMsg(strSmiOid);
	}
}

void CSnmp::SendMsg(LPCTSTR lpszSmiOid)
{
	smiINT sPDUType = SNMP_PDU_GETNEXT;
	if (m_GetOidType == GETOIDTYPE_ONLY)
		sPDUType = SNMP_PDU_GET;

	m_pSnmpMgr->SendSnmpMsg(lpszSmiOid, sPDUType);
}

void CSnmp::SnmpMsgHandler(smiOID sOid, smiVALUE sValue)
{
	CHAR szOid[50];
	smiINT sLen = _countof(szOid);
	SnmpOidToStr(&sOid, sLen, szOid);
	CString strOid(szOid);
	CString strTipInfo;
	CString strOidValue;

	m_pSnmpMgr->ValueToString(sValue, strOidValue);

	if (m_strSmiOidOld.CompareNoCase(strOid) == 0)
		return;

	int nLen = m_strSmiOid.GetLength();
	if (_tcsnicmp(m_strSmiOid, strOid,nLen) != 0)
		return;

	m_strSmiOidOld = strOid;

	CListTextElementUI* pElement = new CListTextElementUI;
	if (pElement)
	{
		m_pList->Add(pElement);
		pElement->SetText(0,strOid);
		pElement->SetText(1,strOidValue);
		TCHAR szLen[20],szType[20];
		ZeroMemory(szType,sizeof(szType));
		GetOidValueLenAndType(sValue,szLen,szType);
		pElement->SetText(2,szLen);
		pElement->SetText(3,szType);
	}

	if (m_strSmiOid.CompareNoCase(strOid) == 0)
		return;

	SendMsg(strOid);
}

void CSnmp::GetOidValueLenAndType(smiVALUE sValue,LPTSTR szLen,LPTSTR szType)
{
	switch(sValue.syntax)
	{
	case SNMP_SYNTAX_NSAPADDR:
	case SNMP_SYNTAX_IPADDR:
		_stprintf_s(szLen,20,_T("%d"),sValue.value.string.len);
		break;
	case SNMP_SYNTAX_OPAQUE:
		_stprintf_s(szLen,20,_T("%d"),4);
		break;
	case SNMP_SYNTAX_OCTETS:
		_stprintf_s(szLen,20,_T("%d"),sValue.value.string.len);
		break;
	case SNMP_SYNTAX_TIMETICKS:
		_stprintf_s(szLen,20,_T("%d"),4);
		break;
	case SNMP_SYNTAX_INT:
		_stprintf_s(szLen,20,_T("%d"),4);
		break;
	case SNMP_SYNTAX_OID:
		_stprintf_s(szLen,20,_T("%d"),sValue.value.oid.len);
		break;
	case SNMP_SYNTAX_UINT32:
	case SNMP_SYNTAX_CNTR32:
	case SNMP_SYNTAX_GAUGE32:
		_stprintf_s(szLen,20,_T("%d"),4);
		break;
	default:
		_stprintf_s(szLen,20,_T("%d"),0);
	}

	LPCTSTR lpszTypeName = _T("");
	switch(sValue.syntax)
	{
	case SNMP_SYNTAX_INT:
		lpszTypeName = _T("INT");
		break;
	case SNMP_SYNTAX_OCTETS:
		lpszTypeName = _T("OCTETS");
		break;
	case SNMP_SYNTAX_OID:
		lpszTypeName = _T("OID");
		break;
	case SNMP_SYNTAX_IPADDR:
		lpszTypeName = _T("IPADDR");
		break;
	case SNMP_SYNTAX_CNTR32:
		lpszTypeName = _T("CNTR32");
		break;
	case SNMP_SYNTAX_GAUGE32:
		lpszTypeName = _T("GAUGE32");
		break;
	case SNMP_SYNTAX_TIMETICKS:
		lpszTypeName = _T("TIMETICKS");
		break;
	case SNMP_SYNTAX_OPAQUE:
		lpszTypeName = _T("OPAQUE");
		break;
	case SNMP_SYNTAX_CNTR64:
		lpszTypeName = _T("CNTR64");
		break;
	case SNMP_SYNTAX_UINT32:
		lpszTypeName = _T("UINT32");
		break;
	}
	_stprintf_s(szType,20,_T("%s[%d]"),lpszTypeName,sValue.syntax);
}