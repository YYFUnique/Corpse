#ifndef BaseDialog_H
#define BaseDialog_H
#include "DuiLib/UIlib.h"

using namespace DuiLib;
 
 class CFileDiskMain : public WindowImplBase
 {
 public:
 	CFileDiskMain();
 	~CFileDiskMain();
 public:
 	virtual LPCTSTR    GetWindowClassName() const;
 	virtual CDuiString GetSkinFile();
 	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
 	virtual void Notify(TNotifyUI& msg);
 	void InitWindow();
 	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
 	CControlUI* CreateControl(LPCTSTR pstrClass);
 protected:
 	DUI_DECLARE_MESSAGE_MAP()
 	void OnClick(TNotifyUI& msg);
 	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
 
 	void OnMenuItemClick(CControlUI* pControl);
 private:
 	CMenuUI* pMenu;
 	CUIShadow m_Shadow;
 };

#endif // BaseDialog_H