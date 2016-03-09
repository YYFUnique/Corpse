#pragma once
#include "UICalendar.h"
#include "UIListTool.h"
class CCalendar : public WindowImplBase
{
public:
	CCalendar();
	~CCalendar();
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
	//virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

protected:
	void OnInitControl();
	void BuildCalendar(UINT nYear,UINT nMonth);
	virtual void OnClick(TNotifyUI& msg);
	void OnItemClick(TNotifyUI& msg);
protected:
	UINT m_nYear;
	UINT m_nMonth;
};