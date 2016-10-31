#include "stdafx.h"
#include "WMITools.h"

#include <Wbemidl.h>
#pragma comment(lib,"Wbemuuid.lib")
CWMITools::CWMITools()
{
	
}

CWMITools::~CWMITools()
{

}

BOOL CWMITools::GetWin32_OperatingSystem(LPCTSTR wszColumnName,CDuiString& strType)
{
	BOOL bSuccess = FALSE;

	IWbemLocator *pWebLoc = NULL;
	IWbemServices *pWebSvc = NULL ;
	do 
	{
		HRESULT hRes = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,IID_IWbemLocator,(LPVOID *) &pWebLoc);
		if( FAILED(hRes)) 
			break;

		hRes = pWebLoc->ConnectServer(_bstr_t("root\\cimv2"),NULL,NULL,0,NULL,0,0,&pWebSvc);
		if( FAILED(hRes))
		{
			pWebLoc->Release();
			pWebLoc = NULL;
			break;
		}

		//设置WMI连接的安全等级。
		hRes = CoSetProxyBlanket(pWebSvc,RPC_C_AUTHN_WINNT,RPC_C_AUTHZ_NONE,NULL,RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE);
		if(FAILED(hRes))
			break;

		IEnumWbemClassObject *pEnumerator = NULL ;
		BSTR bszWQL = SysAllocString ( L"WQL" );
		BSTR bszQuery = SysAllocString (L"Select * from Win32_OperatingSystem");
		if((bszWQL==NULL) || (bszQuery==NULL))
			break;

		hRes = pWebSvc->ExecQuery(bszWQL, bszQuery,WBEM_RETURN_IMMEDIATELY, NULL, &pEnumerator);
		if (FAILED(hRes))
		{
			SysFreeString(bszWQL);
			SysFreeString(bszQuery);
			break;
		}

		SysFreeString(bszWQL);
		SysFreeString(bszQuery);

		ULONG uReturn = 0;
		IWbemClassObject * m_pWbemClass[1] = {NULL};
		hRes = pEnumerator->Next( WBEM_INFINITE, 1, m_pWbemClass, &uReturn);
		if(FAILED(hRes))
			break;

		VARIANT Caption;
		hRes = m_pWbemClass[0]->Get(wszColumnName,	0, &Caption, 0, 0);
		if (FAILED(hRes))
			break;

		//_bstr_t bstr_t= (bstr_t)Caption;
		//strType = (const TCHAR*)bstr_t;

		strType = (LPCTSTR)_bstr_t(Caption);

		bSuccess = TRUE;
	} while (FALSE);

	if (pWebLoc != NULL)
		pWebLoc->Release();
	if (pWebSvc != NULL)
		pWebSvc->Release();

	return bSuccess;
}