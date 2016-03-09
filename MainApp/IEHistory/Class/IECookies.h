#pragma once
#include "CheckItem.h"

class CIeCookies : public CCheckItem
{
public:
	CIeCookies();
	~CIeCookies();
public:
	BOOL GetIeCookies(CItemInfoList& ItemInfoList);
	virtual BOOL Lanuch(CItemInfoList& ItemInfoList);
	virtual BOOL GetListCtrlHeader(CStringArray& strHeader);

protected:

};