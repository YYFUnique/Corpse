#pragma once
#include "ChangeEvent.h"
#include "ITaskbarList3.h"
#include "ObjectArray.h"
#include "ICustomDestinationList.h"
#include <atlstr.h>
#include <ShlObj.h>  
#include <propvarutil.h>  
#include <propkey.h>  

class CPCMaster : public WindowImplBase
{
public:
	CPCMaster();
	~CPCMaster();
//override
public:
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	void SetBkColor(DWORD dwBackColor);
	void HandleCmdLine(LPCTSTR lpszCmdLine);
protected:
	void OnMenuClick(CControlUI* pControl);
	/*DUI_DECLARE_MESSAGE_MAP()*/
		virtual void OnClick(TNotifyUI& msg);
		void OnItemClick(TNotifyUI& msg);
		void OnItemSelect(TNotifyUI& msg);
		LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam);
// 		virtual void OnSelectChanged( TNotifyUI &msg );
// 		virtual void OnItemClick( TNotifyUI &msg );
		void OnAddToobar();
		void AddToolBoard();
		void AddThumbnailButtons();
		void SetMainWndToMapping();
		void OnThumbnailButtonClicked(UINT nBtnId);
		void CreateJumpList();
		HRESULT AddShellLink(IObjectCollection *pOCTasks);
		HRESULT SetTitle( IShellLink * pShellLink, LPCTSTR lpszTitle);
protected:
	SkinChangedObserver skin_changed_observer_;
	ITaskbarList3* m_pTaskbarList;
	UINT				WM_TASKBARBUTTONCREATED;
	HICON				m_hIcon,m_hRed,m_hGreen,m_hBlue;
};