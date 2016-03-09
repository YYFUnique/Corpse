#pragma once

#include "ToolbarMain.h"
typedef CList<CToolbarMain*> CToolbarList;

class CToolbarMgr
{
public:
	CToolbarMgr(void);
	virtual ~CToolbarMgr(void);
	CToolbarMain* GetCurrentToolbar();
	BOOL SetCurrentObjectByID(UINT nID);
	void AddObject(CToolbarMain* pToolbar);
protected:
	CToolbarList m_ToolbarList;
	CToolbarMain* m_pCurrentToolbar;
};

CToolbarMgr* AfxGetToolbarMgr();