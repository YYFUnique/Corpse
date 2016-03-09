#pragma once
#include "CheckItem.h"

class CRunCache : public CCheckItem
{
public:
	CRunCache();
	~CRunCache();
public:
	virtual BOOL Lanuch(CItemInfoList& ItemInfoList);
	virtual BOOL GetListCtrlHeader(CStringArray& strHeader);

protected:

};