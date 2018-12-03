#pragma once
#include "ListViewWnd.h"
#include "Typedef.h"

#include "DllCore/Security/RemoteThreadInject.h"

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
protected:
	void OnClick(TNotifyUI& msg);
	void OnReturn(TNotifyUI& msg);
	void OnKillFocus(TNotifyUI& msg);
	void OnWindowInit(TNotifyUI& msg);
	void OnTextChanged(TNotifyUI& msg);
	void OnFileLookup(TNotifyUI& msg);
protected:
	BOOL EnumProcessesBySnapshot(CProcessInfoList& ProcessInfoList);
	BOOL InjectDll(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId);
private:
	CListViewWnd* m_pListView;
	CRemoteThreadInject m_RemoteInject;
};