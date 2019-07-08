#pragma once

class CISPSInterfaceTest : public CNotifyPump
{
public:
	CISPSInterfaceTest();
	~CISPSInterfaceTest();

public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);

protected:
	BOOL					   m_bLoad;
	CControlBuilder	   m_RootBuilder;
	CDialogBuilder	   m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};