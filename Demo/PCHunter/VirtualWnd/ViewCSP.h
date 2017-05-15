#pragma once
#include <Wincrypt.h>

typedef struct
{
	UINT		nType;
	TCHAR		szName[200];
}PROVIDER_INFO,*PPROVIDER_INFO;

typedef CDuiList <PROVIDER_INFO, const PROVIDER_INFO&> CProviderInfoList;
typedef CDuiList <PROV_ENUMALGS_EX, PROV_ENUMALGS_EX&> CProviderAlgInfo;

class CViewCSP : public CBase
{
public:
	CViewCSP();
	~CViewCSP();

public:
	void OnPaint();

	void GetProviders(CProviderInfoList& ProviderTypes);
	void GetProviderTypes(CProviderInfoList& Providers);
	BOOL GetAlgorithmInfo(LPCTSTR lpszContainer, LPCTSTR lpszProvider, DWORD dwProvType,  CProviderAlgInfo& ProviderAlgList);

protected:

	void GetAlgorithmType(DWORD dwAlgType,LPTSTR lpszAlgType);

	void InsertAlgorithm(CTreeNodeUI* pAlgorithmInfo, const PROV_ENUMALGS_EX& pProvAlg);
	void InsertAlgorithmInfo(CHorizontalLayoutUI* pAlgHori, LPCTSTR lpszItemText);
	CTreeNodeUI* InsertTreeNode(CTreeViewUI* pTreeView, const PROVIDER_INFO& ProviderInfo);
	CTreeNodeUI* InsertSubTreeNode(CTreeNodeUI* pTreeNode, const PROVIDER_INFO& ProviderInfo);
private:
	
};