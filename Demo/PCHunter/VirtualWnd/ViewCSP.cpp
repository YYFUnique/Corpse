#include "StdAfx.h"
#include "../Utils/ErrorInfo.h"
#pragma comment(lib,"Advapi32.lib")


CViewCSP::CViewCSP()
{

}

CViewCSP::~CViewCSP()
{

}

void CViewCSP::OnPaint()
{
	CTreeViewUI* pTreeView = (CTreeViewUI*)m_pPaintManager->FindControl(_T("Csp"));
	if (pTreeView == NULL)
		return;

	CProviderInfoList ProviderTypesList, ProvidersList;

	GetProviderTypes(ProviderTypesList);
	GetProviders(ProvidersList);

	POSITION pos = ProviderTypesList.GetHeadPosition();
	while (pos)
	{
		const PROVIDER_INFO& ProviderTypeInfo = ProviderTypesList.GetNext(pos);
		POSITION ProviderPos = ProvidersList.GetHeadPosition();
		//添加结点
		CTreeNodeUI* pTreeNode = InsertTreeNode(pTreeView, ProviderTypeInfo);

		while (ProviderPos)
		{
			const PROVIDER_INFO& ProvidersInfo = ProvidersList.GetNext(ProviderPos);
			if (ProvidersInfo.nType != ProviderTypeInfo.nType)
				continue;
			//添加子节点
			CTreeNodeUI* pSubNode = InsertSubTreeNode(pTreeNode, ProvidersInfo);

			//添加算法信息
			CProviderAlgInfo ProviderAlgList;
			BOOL bRet = GetAlgorithmInfo(ProviderTypeInfo.szName, ProvidersInfo.szName, ProviderTypeInfo.nType, ProviderAlgList);
			if (bRet == FALSE)
			{
				CTreeNodeUI* pAlgorithmInfo = new CTreeNodeUI;
				pSubNode->Add(pAlgorithmInfo);
				SetErrorInfo(SYSTEM_ERROR,0,_T(""));
				pAlgorithmInfo->SetItemText(GetThreadErrorInfoString());
			}
			else
			{
				POSITION AlgPos = ProviderAlgList.GetHeadPosition();
				while (AlgPos)
				{
					CTreeNodeUI* pAlgorithmInfo = new CTreeNodeUI;
					pSubNode->Add(pAlgorithmInfo);

					const PROV_ENUMALGS_EX& ProvAlg = ProviderAlgList.GetNext(AlgPos);
					InsertAlgorithm(pAlgorithmInfo, ProvAlg);
				}
			}
		}
	}                                                                                                                                                                                                     
}

CTreeNodeUI* CViewCSP::InsertTreeNode(CTreeViewUI* pTreeView, const PROVIDER_INFO& ProviderInfo)
{
	CTreeNodeUI* pCSPType = new CTreeNodeUI;
	if (pCSPType == NULL)
		return NULL;

	LPCTSTR pDefaultAttributes = m_pPaintManager->GetDefaultAttributeList(DUI_CTR_TREENODE);
	if( pDefaultAttributes ) {
		pCSPType->ApplyAttributeList(pDefaultAttributes);
	}

	pTreeView->Add(pCSPType);

	TCHAR szProviderTypes[MAX_PATH];
	_stprintf_s(szProviderTypes,_countof(szProviderTypes), _T("Provider 类型 : %s[%d]"), 
																			ProviderInfo.szName, ProviderInfo.nType);

	pCSPType->SetItemText(szProviderTypes);
	pCSPType->GetItemButton()->SetFont(0);
	pCSPType->SetFixedHeight(33);

	return pCSPType;
}

CTreeNodeUI* CViewCSP::InsertSubTreeNode(CTreeNodeUI* pTreeNode, const PROVIDER_INFO& ProviderInfo)
{
	CTreeNodeUI* pCSPName = new CTreeNodeUI;
	pTreeNode->Add(pCSPName);

	pCSPName->SetItemText(ProviderInfo.szName);
	pCSPName->GetItemButton()->SetFont(0);
	pCSPName->SetFixedHeight(28);

	return pCSPName;
}

void CViewCSP::InsertAlgorithm(CTreeNodeUI* pAlgorithmInfo, const PROV_ENUMALGS_EX& pProvAlg)
{
	CHorizontalLayoutUI* pHori = pAlgorithmInfo->GetTreeNodeHoriznotal();
	TCHAR szAlgorithmInfo[MAX_PATH];

	//添加算法名称
	pAlgorithmInfo->SetItemText(CString(pProvAlg.szName));
	//添加算法全称
	//InsertAlgorithmInfo(pHori, CString(pProvAlg.szLongName));
	//添加算法类型
	GetAlgorithmType(pProvAlg.aiAlgid, szAlgorithmInfo);
	InsertAlgorithmInfo(pHori, szAlgorithmInfo);
	//默认密钥长度
	_stprintf_s(szAlgorithmInfo, _countof(szAlgorithmInfo), _T("%d"), pProvAlg.dwDefaultLen);
	InsertAlgorithmInfo(pHori, szAlgorithmInfo);
	//最大最小密钥长度
	UINT nMax = pProvAlg.dwMaxLen;
	if (pProvAlg.dwMaxLen == 0)
		nMax = 0;
	_stprintf_s(szAlgorithmInfo, _countof(szAlgorithmInfo), _T("%d~%d"), pProvAlg.dwMinLen, nMax);
	InsertAlgorithmInfo(pHori, szAlgorithmInfo);
	//算法ID
	_stprintf_s(szAlgorithmInfo, _countof(szAlgorithmInfo), _T("%8.8xH"), pProvAlg.aiAlgid);
	InsertAlgorithmInfo(pHori, szAlgorithmInfo);
}

void CViewCSP::InsertAlgorithmInfo(CHorizontalLayoutUI* pAlgHori, LPCTSTR lpszItemText)
{
	CLabelUI* pLabel = new CLabelUI;
	pAlgHori->Add(pLabel);
	
	pLabel->SetText(lpszItemText);
}

void CViewCSP::GetProviderTypes(CProviderInfoList& ProviderTypes)
{
	DWORD dwProvType = 0;
	DWORD dwIndex = 0;

	TCHAR szName[MAX_PATH];
	DWORD cbName = _countof(szName);

	while (CryptEnumProviderTypes(dwIndex++, NULL, 0, &dwProvType, szName, &cbName))
	{
		PROVIDER_INFO ProviderInfo;

		ProviderInfo.nType = dwProvType;

		memcpy_s(ProviderInfo.szName, _countof(ProviderInfo.szName), szName, cbName+1);
		ProviderTypes.AddTail(ProviderInfo);

		cbName = _countof(szName);
	}
}

void CViewCSP::GetProviders(CProviderInfoList& Providers)
{
	DWORD dwItem = 0;

	TCHAR   szProvider[MAX_PATH];
	DWORD dwFlag = 0;
	DWORD dwLen = _countof(szProvider);
	while(CryptEnumProviders(dwItem++, NULL, 0, &dwFlag, szProvider, &dwLen))
	{
		PROVIDER_INFO ProviderInfo;
		ProviderInfo.nType = dwFlag;
		memcpy_s(ProviderInfo.szName, _countof(ProviderInfo.szName), szProvider, dwLen +1);

		Providers.AddTail(ProviderInfo);
		dwLen = _countof(szProvider);
	}
}

BOOL CViewCSP::GetAlgorithmInfo(LPCTSTR lpszContainer, LPCTSTR lpszProvider, DWORD dwProvType, CProviderAlgInfo& ProviderAlgList)
{
	HCRYPTPROV hProv = NULL;
	BOOL bSuccess = FALSE;
	do 
	{
		BOOL bRet = CryptAcquireContext(&hProv, lpszContainer, lpszProvider, dwProvType, CRYPT_SILENT);
		if (bRet == FALSE && GetLastError() == NTE_BAD_KEYSET)
			bRet = CryptAcquireContext(&hProv, lpszContainer, lpszProvider, dwProvType, CRYPT_SILENT|CRYPT_NEWKEYSET);
		
		if (bRet == FALSE)
			break;

		BYTE pbData[1024];
		DWORD cbData = _countof(pbData);
		DWORD dwCryptFlag = CRYPT_FIRST;

		while (CryptGetProvParam(hProv, PP_ENUMALGS_EX, pbData, &cbData, dwCryptFlag))
		{
			dwCryptFlag = CRYPT_NEXT;
			PROV_ENUMALGS_EX* pProvParam = (PROV_ENUMALGS_EX*)pbData;
			ProviderAlgList.AddTail(*pProvParam);
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hProv != NULL)
	{
		CryptReleaseContext(hProv, 0);
		hProv = NULL;
	}
	
	return bSuccess;
}

void CViewCSP::GetAlgorithmType(DWORD dwAlgType,LPTSTR szAlgType)
{
	LPCTSTR lpszAlgType = NULL;
	switch(GET_ALG_CLASS(dwAlgType))
	{
	case ALG_CLASS_DATA_ENCRYPT: 
			lpszAlgType = _T("Encrypt");
		break;
	case ALG_CLASS_HASH:         
			lpszAlgType = _T("Hash");
		break;
	case ALG_CLASS_KEY_EXCHANGE: 
			lpszAlgType = _T("Exchange");
		break;
	case ALG_CLASS_SIGNATURE:    
			lpszAlgType = _T("Signature");
		break;
	case ALG_CLASS_MSG_ENCRYPT:
			lpszAlgType = _T("Msg Encrypt");
		break;
	default:
			lpszAlgType = _T("Unknown");
		break;
	}
	_stprintf_s(szAlgType, 20, _T("%s"), lpszAlgType);
}