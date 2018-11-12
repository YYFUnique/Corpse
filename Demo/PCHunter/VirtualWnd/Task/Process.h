#pragma once
#include "DllCore/Utils/StdPtr.h"
#include "DllCore/Authority/Process.h"

class CProcess : public CNotifyPump
{
public:
	CProcess();
	~CProcess();

public:
	void SetPaintMagager(CPaintManagerUI *pPaintMgr);
protected:
	void CreateProcessItem(CListUI* pList, const TListInfoUI* pListInfo, const PROCESS_ITEM_INFO& ProcessInfo);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnRefresh(TNotifyUI& msg);
	void OnTimerEx(TNotifyUI& msg);
	void OnLoadItem(TNotifyUI& msg);
	void OnItemActive(TNotifyUI& msg);
private:
	HICON					m_hAppIcon;
	BOOL					m_bLoad;
	DWORD				m_dwLastTick;
	DWORD				m_dwCpuCount;
	CStdStringPtrMap	m_strFileDescInfo;
	CControlBuilder	m_RootBuilder;
	CDialogBuilder m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};