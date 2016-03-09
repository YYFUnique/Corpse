#pragma once
#include "CheckItem.h"

class CIeAddrCache : public CCheckItem
{
public:
	CIeAddrCache();
	~CIeAddrCache();
public:
	BOOL GetIeAddrCache(LPCTSTR lpszIeAddrCache,CItemInfoList& ItemInfoList);
	virtual BOOL Lanuch(CItemInfoList& ItemInfoList);
	virtual BOOL GetListCtrlHeader(CStringArray& strHeader);

protected:

};