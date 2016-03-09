#pragma once
#include "../IEHistoryModule.h"
#include "LsCommon/Browser/Browser.h"

class CCheckItem
{
public:
	CCheckItem();
	virtual ~CCheckItem();
public:
	virtual BOOL Lanuch(CItemInfoList& ItemInfoList) = 0;
	virtual BOOL GetListCtrlHeader(CStringArray& strHeader) = 0;
	BOOL GetItemDescribe(CString& strDescribe);
	static CCheckItem* CreateCheckItem(MODULE_TYPE ModuleType);
	virtual void Release();
protected:
	CString m_strDescribe;
private:
};