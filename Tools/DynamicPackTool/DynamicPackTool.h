#pragma once

#include "Solution.h"

class CDynamicPackTool : public WindowImplBase
{
public:
	CDynamicPackTool();
	~CDynamicPackTool();

public:
	void OnFinalMessage( HWND hWnd );
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual LPCTSTR GetResourceID() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();

	CControlUI* CreateControl(LPCTSTR pstrClass);
public:
	void OutputProcInfo(LPCTSTR lpszDbgInfo);

protected:
	// 编译相关函数
	LRESULT	OnExecute();
	LRESULT	OnBuildRun();
protected:
	//消息相关
	void OnClick(TNotifyUI& msg);
	void OnDbClick(TNotifyUI& msg);
	void OnItemClick(TNotifyUI& msg);
	void OnMenuClick(TNotifyUI& msg);
	void OnMenuItem(CControlUI* pControl);
	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	CSolution*	m_pSolutionTool;
};