#include "StdAfx.h"
#include "CheckItem.h"

#include "Histroy.h"
#include "IECookies.h"
#include "IECache.h"
#include "ExplorerCache.h"
#include "RunCache.h"
#include "IEAddrCache.h"

CCheckItem::CCheckItem()
{

}

CCheckItem::~CCheckItem()
{
	//Release();
}

CCheckItem* CCheckItem::CreateCheckItem(MODULE_TYPE ModuleType)
{
	CCheckItem* pCheckItem = NULL;
	switch(ModuleType)
	{
		case MODULE_TYPE_History:
				pCheckItem = new CHistory;
			break;
		case MODULE_TYPE_IECache:
				pCheckItem = new CIeCache;
			break;
		case MODULE_TYPE_ExplorerCache:
				pCheckItem = new CExplorerCache;
			break;
		case MODULE_TYPE_Cookies:
				pCheckItem = new CIeCookies;
			break;
		case MODULE_TYPE_RunCache:
				pCheckItem = new CRunCache;
			break;
		case MODULE_TYPE_AddressCache:
				pCheckItem = new CIeAddrCache;
			break;
	}

	return pCheckItem;
}

void CCheckItem::Release()
{
	delete this;
}

BOOL CCheckItem::GetItemDescribe(CString& strDescribe)
{
	strDescribe = m_strDescribe;
	return TRUE;
}