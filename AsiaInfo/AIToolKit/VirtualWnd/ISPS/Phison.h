#pragma once

#include "PhisonSDK_FF01/PhisonSDK_FF01.h"
#include "DllCore/Sync/SyncLock.h"

using namespace TMPS::Firmware;

typedef struct tagFILE_MODIFY_INFO
{
	int nModifyTick;
	int nCaptured;
}FILE_MODIFY_INFO;

typedef std::map<LPCTSTR, FILE_MODIFY_INFO> CMapFileModified;
typedef std::vector<LPCTSTR> PathCollection;

class CPhison : public CNotifyPump
{
public:
	CPhison();
	~CPhison();

public:
	void SetPaintManager(CPaintManagerUI* pPaintMgr);

protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnClick(TNotifyUI& msg);
		void OnLoadItem(TNotifyUI& msg);
protected:
	void OnBtnRefresh();
	void OnBtnRead();
	void OnBtnReadTmp();
	void OnBtnWrite();
	void OnBtnWriteTmp();
	void OnBtnFormat();
	void OnBtnAdjust();
	void EnableWindows(bool bEnable = TRUE);
protected:

protected:
	BOOL					   m_bLoad;
	CControlBuilder	   m_RootBuilder;
	CDialogBuilder	   m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
private:
	//CDirectoryChangeWatcher* m_pDirectoryWatcher;
	//CDirectoryChangeHandlerEx* m_pDirectoryHandler;
	CMapFileModified  m_FileModifiedMap;
	PhisonSDK_FF01*	m_pPhisonSDK;
};