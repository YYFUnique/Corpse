#pragma once
#include "ProcessInfo.h"

class CProcessDetailInfo : public CProcessInfo
{
public:
	CProcessDetailInfo();
	//virtual ~CProcessDetailInfo();
	virtual BOOL GetListColumn(CStringArray& strListColumn);
	virtual BOOL ShowProcessInfo(CListCtrl& ListCtrl);
	void SetProcessProperty(CListCtrl& ListCtrl);
	virtual BOOL OnRButtonDown(UINT nID);
};