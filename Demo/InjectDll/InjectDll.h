#pragma once

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

protected:
	void OnClick(TNotifyUI& msg);
	void OnFileLookup(TNotifyUI& msg);
protected:

	BOOL InjectDll(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId);
private:
	CRemoteThreadInject m_RemoteInject;
};