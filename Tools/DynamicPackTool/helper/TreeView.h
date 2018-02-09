#pragma once

#include "../Project.h"

class CSolution;
class CProjectTreeView
{
public:
	CProjectTreeView(CSolution* pSolution, CPaintManagerUI* pManagerUI);
	~CProjectTreeView();

public:
	BOOL LoadSolution();
	BOOL SetSelectItemStart();
	BOOL GetActiveProjectGUID(CString& strActiveProjectGUID);
protected:
	void InsertProjectDetailInfo(CTreeNodeUI* pProjectNode, PROJECT_DETAILS_INFO& ProjectDetailInfo);
protected:
	CSolution*				m_pSolution;
	CPaintManagerUI*	m_pManagerUI;
	CTreeNodeUI*			m_pActiveNode;
};