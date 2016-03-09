#pragma once 
#include "ProcessInfo.h"

class CProcessServiceInfo : public CProcessInfo
{
public:
	CProcessServiceInfo();
	//virtual ~CProcessServiceInfo();
	virtual BOOL GetListColumn(CStringArray& strListColumn);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl);
};