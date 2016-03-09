#pragma once
#include "ChangeEvent.h"
#include "ITaskbarList3.h"
#include "ObjectArray.h"
#include "ICustomDestinationList.h"
#include <atlstr.h>
#include <ShlObj.h>  
#include <propvarutil.h>  
#include <propkey.h>  
//#include "UiFeatureEffect/Interface/IUIEffect.h"

#include "CDrawingBoard.h"

class CPCMaster : public WindowImplBase//,public IUIEffectCallBack
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
	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	void SetBkColor(DWORD dwBackColor);
	void SetBkImage(LPCTSTR lpszPathFileName);
	void AddToolBar(LPCTSTR lpszToolBar,LPCTSTR lpszDisplayName);
	void HandleCmdLine(LPCTSTR lpszCmdLine);

	virtual void OnUiEffectBegin(WPARAM effectKey, DWORD animaType);
	virtual void OnUiEffectEnd(WPARAM effectKey, DWORD animaType);
	virtual void OnUiEffectDraw();
protected:
	void OnMenuClick(CControlUI* pControl);
	/*DUI_DECLARE_MESSAGE_MAP()*/
		virtual void OnClick(TNotifyUI& msg);
		//virtual LRESULT OnClose(UINT , WPARAM , LPARAM , BOOL& bHandled);
		void OnItemClick(TNotifyUI& msg);
		void OnItemSelect(TNotifyUI& msg);
		void OnMenu(TNotifyUI& msg);
		LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam);
// 		virtual void OnSelectChanged( TNotifyUI &msg );
// 		virtual void OnItemClick( TNotifyUI &msg );
		void OnLoadToobar();
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

	//IUIEffect* m_pEffect;

	//CDrawingBoard* m_pDraw;
};