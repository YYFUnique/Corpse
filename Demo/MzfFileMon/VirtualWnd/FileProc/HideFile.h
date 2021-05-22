#pragma once
#include "Utils/ControlBuilder.h"
#include "DuiLib/Core/UIDlgBuilder.h"
#include <DrvControl/FsFltIoctl.h>

class CHideFile : public CNotifyPump
{
public:
	CHideFile();
	~CHideFile();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);
	void OnAppMenu(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnMenu(TNotifyUI& msg);
		void OnLoadItem(TNotifyUI& msg);
protected:
	void AddItem();
	void LoadItem();
	void DeleteItem();
	void StopFilter();
	void CreateAndShowListItem(CListUI* pList, DWORD dwIndex, USER_FILE_PATH_RULE* pUserFilePathRule);
protected:
	static CString GetHideFileMode(MATCH_MODE MatchMode);
private:
	BOOL						m_bLoad;
	CFileSystemFilter*	m_pFsFltDrv;
	CControlBuilder		m_RootBuilder;
	CDialogBuilder		m_DialogBuilder;
	CPaintManagerUI*  m_pPaintManager;
};