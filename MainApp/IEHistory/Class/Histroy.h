#pragma once
#include "CheckItem.h"

class CHistory : public CCheckItem
{
public:
	CHistory();
	~CHistory();
public:
	virtual BOOL Lanuch(CItemInfoList& ItemInfoList);
	virtual BOOL GetListCtrlHeader(CStringArray& strHeader);

protected:
	BOOL GetHistoryInfo(CItemInfoList& ItemInfoList);
};