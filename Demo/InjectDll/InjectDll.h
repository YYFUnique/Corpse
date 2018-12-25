#pragma once
#include "ListViewWnd.h"
#include "Typedef.h"

#include "DllCore/Security/RemoteThreadInject.h"

typedef struct tagINJECT_INFO 
{
	DWORD	dwProcessId;			// 被注入进程ID
	HANDLE	hEvent;						// 事件句柄
	CString		strEvent;					// 全局事件名称
}INJECT_INFO, *PINJECT_INFO;

typedef CDuiList <INJECT_INFO, const INJECT_INFO&> CInjectInfoList;

class CInjectDll : public WindowImplBase
{
public:
	CInjectDll();
	~CInjectDll();

public:
	virtual void OnFinalMessage(HWND hWnd);
	virtual LPCTSTR    GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetZIPFileName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	void OnClick(TNotifyUI& msg);
	void OnReturn(TNotifyUI& msg);
	void OnKillFocus(TNotifyUI& msg);
	void OnWindowInit(TNotifyUI& msg);
	void OnTextChanged(TNotifyUI& msg);
	void OnFileLookup(TNotifyUI& msg);
protected:
	BOOL EnumProcessesBySnapshot(CProcessInfoList& ProcessInfoList);
	BOOL InjectDll(DWORD dwRemoteProcessId, LPCTSTR lpszFilePath);
private:
	CListViewWnd* m_pListView;
	CInjectInfoList		m_InjectInfoList;
	CRemoteThreadInject* m_pRemoteInject;
};