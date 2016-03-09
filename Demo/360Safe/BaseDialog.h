#ifndef BaseDialog_H
#define BaseDialog_H

#include "DuiLib/UIlib.h"

#include <map>

using namespace DuiLib;

class BaseDialog : public CWindowWnd, public INotifyUI
{

public:
	BaseDialog();

protected:
	virtual LPCTSTR GetWindowClassName() const { return _T("360Safe Main Dialog"); }

	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// custom message map
	typedef HRESULT (BaseDialog::*CustomMsgHandler)(WPARAM, LPARAM, BOOL&);
	typedef std::map<UINT, CustomMsgHandler> MessageMap;
	virtual MessageMap* InitMessageMap();

	// custom message handler
	HRESULT OnCreate(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnDestory(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnErasebkgnd(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnNcPaint(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnNcActive(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnNcCalSize(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CPaintManagerUI m_pm;
};

#endif // BaseDialog_H