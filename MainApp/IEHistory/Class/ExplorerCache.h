#pragma once
#include "CheckItem.h"

class CExplorerCache : public CCheckItem
{
public:
	CExplorerCache();
	~CExplorerCache();
public:
	virtual BOOL Lanuch(CItemInfoList& ItemInfoList);
	virtual BOOL GetListCtrlHeader(CStringArray& strHeader);

protected:

};