#pragma once
#include "CheckItem.h"

class CIeCache : public CCheckItem
{
public:
	CIeCache();
	~CIeCache();
public:
	virtual BOOL Lanuch(CItemInfoList& ItemInfoList);
	virtual BOOL GetListCtrlHeader(CStringArray& strHeader);

protected:

};