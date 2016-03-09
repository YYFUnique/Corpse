#include "stdafx.h"
#include <Ws2spi.h>
#pragma comment(lib,"Ws2_32.lib")

CProvider::CProvider()
{
	/*m_bTimer = TRUE;*/
}

CProvider::~CProvider()
{

}

void CProvider::OnPaint()
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Provider"));
	if (pList->GetCount())
		pList->RemoveAll();

	CWsaProtocolInfoList WsaProtocolList;
	EnumProtocols(WsaProtocolList);

	POSITION pos = WsaProtocolList.GetHeadPosition();
	int nIndex = 0;
	while(pos)
	{
		CListTextElementUI* pTextElement = new CListTextElementUI;
		pList->Add(pTextElement);

		const WSAPROTOCOL_INFOW& WsaProtocolInfo = WsaProtocolList.GetNext(pos);
		int m=0;
		TCHAR szNum[15];
		pTextElement->SetText(m++,_itot(++nIndex,szNum,10));
		pTextElement->SetText(m++,WsaProtocolInfo.szProtocol);
		pTextElement->SetText(m++,_itot(WsaProtocolInfo.dwCatalogEntryId,szNum,10));
		pTextElement->SetText(m++,_itot(WsaProtocolInfo.iVersion,szNum,10));
		pTextElement->SetText(m++,GetAddressFamily(WsaProtocolInfo.iAddressFamily));
		IPPROTO IpProto = (IPPROTO)WsaProtocolInfo.iProtocol;
		pTextElement->SetText(m++,GetAddressProtocol(WsaProtocolInfo.iProtocol));
		pTextElement->SetText(m++,_itot(WsaProtocolInfo.ProtocolChain.ChainLen,szNum,10));

		TCHAR szProviderPath[MAX_PATH];
		int nSize = _countof(szProviderPath);
		int nErrorCode = ERROR_SUCCESS;
		DWORD dwRet = WSCGetProviderPath((LPGUID)&WsaProtocolInfo.ProviderId,szProviderPath,&nSize,&nErrorCode);
		if (dwRet != SOCKET_ERROR)
			pTextElement->SetText(m++,szProviderPath);
	}
}

BOOL CProvider::EnumProtocols(CWsaProtocolInfoList& WsaProtocolInfo)
{
	BOOL bSuccess = FALSE;

	LPWSAPROTOCOL_INFOW lpWsaProtocolInfo =NULL;
	do 
	{
		DWORD dwSize = 0;
		int nErrorCode = ERROR_SUCCESS;
		int nCount = WSCEnumProtocols(NULL,lpWsaProtocolInfo,&dwSize,&nErrorCode);
		if (nErrorCode == WSAENOBUFS)
		{
			lpWsaProtocolInfo = (LPWSAPROTOCOL_INFOW)new BYTE[dwSize];
			if (lpWsaProtocolInfo == NULL)
				break;

			nErrorCode = ERROR_SUCCESS;

			nCount = WSCEnumProtocols(NULL,lpWsaProtocolInfo,&dwSize,&nErrorCode);
		}

		if (nErrorCode == WSAENOBUFS)
			break;

		for (int n=0;n<nCount;++n)
			WsaProtocolInfo.AddTail(lpWsaProtocolInfo[n]);

		bSuccess = TRUE;
	} while (FALSE);

	if (lpWsaProtocolInfo != NULL )
	{
		delete[] lpWsaProtocolInfo;
		lpWsaProtocolInfo = NULL;
	}

	return bSuccess;
}